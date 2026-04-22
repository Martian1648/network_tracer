#include "bvh.h"
#include "constants.h"
#include "random.h"
#include "hit.h"
#include <algorithm>

BVH::BVH(const std::vector<std::unique_ptr<Object>>& objects) {
    // Nodes of the BVH only need a pointer to the object, not to maintain ownership.
    // Build a list of raw object pointers
    std::vector<Object*> object_pointers;
    for (const auto& obj : objects) {
        object_pointers.push_back(obj.get());
    }
    root = build_tree(object_pointers);
}

std::optional<Hit> BVH::find_nearest(const Ray& ray) const {
    // test intersection with the root node
    auto test = intersect(root, ray);
    // if you hit something, then construct a hit on the object
    if (test.second.has_value()) {
        return test.first->construct_hit(ray, test.second.value());

    }
    // otherwise no hit
    else {
        return {};
    }
}


BVH::Node* BVH::build_tree(std::vector<Object*> objects) const {
    // Recursively build a tree
    Node* n = new Node{};
    // at leaf nodes, set the node object and bounding box
    if (objects.size() == 1) {
        n->object = objects[0];
        n->box = objects[0]->bounding_box();
    }
    // for all others partition and recursively call build_tree
    else {
        auto [left, right] = partition_along_random_axis(objects);
        n->left = build_tree(left);
        n->right = build_tree(right);
        n->box = surrounding_box(n->left->box, n->right->box);
    }
    // make sure to handle surrounding boxes at non-leaf nodes
    return n;
}


bool BVH::compare_boxes(Object* a, Object* b, int axis) const {
    // For the given axis (x=0, y=1, z=2) returns whether a's box is less than b's
    Point3D point_a = a->bounding_box().minimum;
    Point3D point_b = b->bounding_box().minimum;
    if (axis == 0) {
        return point_a.x < point_b.x;
    }
    else if (axis == 1) {
        return point_a.y < point_b.y;
    }
    else {
        return point_a.z < point_b.z;
    }
}

std::pair<std::vector<Object*>, std::vector<Object*>> BVH::partition_along_random_axis(std::vector<Object*> objects) const {
    // sort objects along a random axis
    int axis = std::round(random_double(0,2));
    std::sort(objects.begin(), objects.end(),
        [this, axis](Object* a, Object* b) {return compare_boxes(a, b,axis);});
    // divide objects vector into two vectors
    size_t middle = objects.size() / 2;
    std::vector<Object*> left(objects.begin(), objects.begin() + middle);
    std::vector<Object*> right(objects.begin() + middle, objects.end());
    return {left, right};
}

std::pair<Object*, std::optional<double>> BVH::intersect(Node* n, const Ray& ray) const {
    // First handle the ray not intersection the box, then handle leaf nodes
    if (!n->box.intersect(ray)) {
        return {nullptr, {}};
    }
    if (n->object != nullptr) {
        auto t = n->object->intersect(ray);

        return {n->object, t};
    }
    // Remember that boxes can overlap, so you must test both left and right boxes.
    auto left = intersect(n->left, ray);
    auto right = intersect(n->right, ray);
    // Given the results of hitting possible left or right or both, return the closest

    if (left.second.has_value() && right.second.has_value()) {
        if (left.second.value()<right.second.value()) {
            return left;
        }
        return right;
    }
    else if (left.second.has_value()) {
        return left;
    }
    else if (right.second.has_value()) {
        return right;
    }
    else {
        return{nullptr, {}};
    }

}