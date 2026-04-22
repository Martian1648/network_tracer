/*
ὄνομα: swirl
αἰών: 3/31/2025
βούλημα: raytracer
*/

#include "swirl.h"
#include <cmath>

Swirl::Swirl(Color primary, Color secondary, Color tertiary, Color base)
    :primary{primary}, secondary{secondary}, tertiary{tertiary}, base{base}{

}

Color Swirl::value(double u, double v) const {
    /*
     *I did in fact look this up, because I had no clue, but I do understand how it works, this is
     * *mostly* not a copy and paste job
     */
    double num_swirls = 2; //number of times any given swirl makes a full rotation around the sphere
    double threshold = 1.0/8.0; //essentially how *thick* a swirl may be, although it works a bit more like epsilon
    auto swirl = u + num_swirls * v;
    /* The swirl value is that with any given point on the sphere,
     * as v goes to 1, num swirls gets larger and larger, and is shifted by u
     */
    swirl = swirl - std::floor(swirl); //then it is broght back to the [0,1) range

    double d0 = std::min(std::fabs(swirl-0.0), 1 - std::fabs(swirl-0.0));
    //this gets the absolute value of the distance between the current point and
    //a point where it could be a color. But since its not like a normal line
    //it works differently. Think of a clock at 11:00. Is it 11 hours away from noon
    //or just one? This is where I just kinda copied off the internet.
    double d1 = std::min(std::fabs(swirl-1.0/3.0), 1 - std::fabs(swirl-1.0/3.0));
    double d2 = std::min(std::fabs(swirl-2.0/3.0), 1 - std::fabs(swirl-2.0/3.0));

    //then if the distance between any point and its 'true spiral' is within the allowed
    //threshold, it returns the color
    if (d0 < threshold) {
        return primary;
    }
    if (d1 < threshold) {
        return secondary;
    }
    if (d2 < threshold) {
        return tertiary;
    }
    return base;

}