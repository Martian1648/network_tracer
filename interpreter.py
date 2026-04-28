"""
Scene command shell.

Import this module and call `run(line)` with one space-separated command.
Returns a dict whose shape matches the C++ JsonParser schema exactly:

  - ADD/EDIT/DELETE: {action, type, name, ...fields}
  - SET:             {action, target, ...fields}
  - GET:             {action, target}
  - QUIT:            {action}

Raises ShellError on bad input.
"""

import shlex


class ShellError(Exception):
    """Raised on any malformed command."""


# ---------------------------------------------------------------------------
# Token helpers
# ---------------------------------------------------------------------------

def _vector(tokens, label):
    """Pop a '(a b c)' vector from tokens — quoted or split across tokens."""
    if not tokens:
        raise ShellError(f"missing vector for {label}")

    first = tokens[0]

    # Single quoted token: "(1 2 3)"
    if first.startswith("(") and first.endswith(")") and len(first) > 2:
        parts = first[1:-1].split()
        tokens.pop(0)
    # Three separate tokens: '(1' '2' '3)'
    elif first.startswith("("):
        if len(tokens) < 3 or not tokens[2].endswith(")"):
            raise ShellError(f"malformed vector for {label}")
        parts = [tokens[0][1:], tokens[1], tokens[2][:-1]]
        del tokens[0:3]
    else:
        raise ShellError(f"vector for {label} must start with '('")

    if len(parts) != 3:
        raise ShellError(f"vector for {label} needs 3 numbers")
    try:
        return [float(p) for p in parts]
    except ValueError:
        raise ShellError(f"vector for {label} must be numeric") from None


def _pop(tokens, label, cast=str):
    if not tokens:
        raise ShellError(f"missing value for {label}")
    raw = tokens.pop(0)
    try:
        return cast(raw)
    except ValueError:
        raise ShellError(f"bad value for {label}: {raw!r}") from None


def _bool(value):
    low = value.lower()
    if low in ("true", "t", "yes", "y", "1"):
        return True
    if low in ("false", "f", "no", "n", "0"):
        return False
    raise ValueError("expected boolean")


def _done(tokens, where):
    if tokens:
        raise ShellError(f"extra arguments for {where}: {tokens}")


# ---------------------------------------------------------------------------
# Per-type field parsers
#
# Each returns ONLY the details fields (no name, no type, no action) — those
# are attached at the top level by the caller. This matches what strip_routing
# leaves in the C++ details maps.
# ---------------------------------------------------------------------------

def _texture_fields(tokens):
    tt = _pop(tokens, "texture_type")
    fields = {"texture_type": tt}

    if tt == "solid":
        fields["color"] = _vector(tokens, "color")
    elif tt == "gradient":
        fields["color1"] = _vector(tokens, "color1")
        fields["color2"] = _vector(tokens, "color2")
        fields["vertical"] = _pop(tokens, "vertical", _bool)
    elif tt == "checkerboard":
        fields["color1"] = _vector(tokens, "color1")
        fields["color2"] = _vector(tokens, "color2")
    elif tt == "swirl":
        fields["color1"] = _vector(tokens, "color1")
        fields["color2"] = _vector(tokens, "color2")
        fields["color3"] = _vector(tokens, "color3")
        fields["color4"] = _vector(tokens, "color4")
    elif tt in ("normal", "marble"):
        pass
    else:
        raise ShellError(f"unknown texture_type: {tt!r}")

    _done(tokens, "texture")
    return fields


def _material_fields(tokens):
    mt = _pop(tokens, "material_type")
    fields = {
        "material_type": mt,
        "texture_name": _pop(tokens, "texture_name"),
        "emitting": _pop(tokens, "emitting", _bool),
    }
    if mt == "metallic":
        fields["fuzz"] = _pop(tokens, "fuzz", float)
    _done(tokens, "material")
    return fields


def _object_fields(tokens):
    fields = {
        "center": _vector(tokens, "center"),
        "radius": _pop(tokens, "radius", float),
        "material_name": _pop(tokens, "material_name"),
        "angle": _vector(tokens, "angle"),
    }
    _done(tokens, "object")
    return fields


_TYPES = {
    "texture":  _texture_fields,
    "material": _material_fields,
    "object":   _object_fields,
}


# ---------------------------------------------------------------------------
# Action handlers
# ---------------------------------------------------------------------------

def _add_or_edit(action, tokens):
    t = _pop(tokens, "type")
    if t not in _TYPES:
        raise ShellError(f"unknown type: {t!r}")
    name = _pop(tokens, "name")
    packet = {"action": action, "type": t, "name": name}
    packet.update(_TYPES[t](tokens))
    return packet


def _delete(tokens):
    t = _pop(tokens, "type")
    if t not in _TYPES:
        raise ShellError(f"unknown type: {t!r}")
    name = _pop(tokens, "name")
    _done(tokens, "DELETE")
    return {"action": "DELETE", "type": t, "name": name}


def _get(tokens):
    target = _pop(tokens, "target")
    packet = {"action": "GET", "target": target}
    if target == "object" or target == "material" or target == "texture":
        packet['name'] = _pop(tokens, "name")
    _done(tokens, "GET")
    return packet

def _set(tokens):
    target = _pop(tokens, "target")
    packet = {"action": "SET", "target": target}

    if target == "camera":
        packet["position"]     = _vector(tokens, "position")
        packet["target_point"] = _vector(tokens, "target_point")
        packet["up"]           = _vector(tokens, "up")
        packet["fov"]          = _pop(tokens, "fov", float)
    elif target == "output":
        # build_misc reads misc_details["name"]["name"], so this key must
        # literally be "name" — not "filename" or "output".
        packet["name"] = _pop(tokens, "name")
    elif target == "rays":
        packet["ray_depth"]   = _pop(tokens, "ray_depth", int)
        packet["ray_samples"] = _pop(tokens, "ray_samples", int)

    else:
        raise ShellError(f"unknown SET target: {target!r}")

    _done(tokens, f"SET {target}")
    return packet


# ---------------------------------------------------------------------------
# Public entry point
# ---------------------------------------------------------------------------

def run(line):
    """Parse one command line into a JSON-serializable dict.

    Raises ShellError on empty or malformed input.
    """
    try:
        tokens = shlex.split(line)
    except ValueError as exc:
        raise ShellError(f"tokenization failed: {exc}") from None

    if not tokens:
        raise ShellError("empty command")

    action = tokens.pop(0).upper()
    if action in ("ADD", "EDIT"):
        return _add_or_edit(action, tokens)
    if action == "DELETE":
        return _delete(tokens)
    if action == "GET":
        return _get(tokens)
    if action == "SET":
        return _set(tokens)
    if action == "QUIT":
        return {"action": "QUIT"}
    raise ShellError(f"unknown action: {action!r}")

def print_dict(d, indent=0):
    """
    Pretty-print a dictionary with custom formatting rules:
    - Objects (dicts) are wrapped in curly brackets, each key on its own line
    - Basic types and arrays of exactly 3 numbers print inline after the key
    - Other arrays are wrapped in square brackets, one element per line
    """
    pad = "    " * indent
    inner_pad = "    " * (indent + 1)

    print(pad + "{")
    for key, value in d.items():
        print(f'{inner_pad}"{key}" : ', end="")
        _print_value(value, indent + 1, newline_before=False)
    print(pad + "}")


def _print_value(value, indent, newline_before=True):
    pad = "    " * indent
    inner_pad = "    " * (indent + 1)

    if isinstance(value, dict):
        # Object: curly brackets, recurse
        print()
        print(pad + "{")
        for key, val in value.items():
            print(f'{inner_pad}"{key}" : ', end="")
            _print_value(val, indent + 1, newline_before=False)
        print(pad + "}")
    elif isinstance(value, list):
        if _is_three_numbers(value):
            # Array of exactly 3 numbers: inline
            print(value)
        else:
            # Other arrays: square brackets, one per line
            print()
            print(pad + "[")
            for item in value:
                if isinstance(item, (dict, list)):
                    _print_value(item, indent + 1, newline_before=True)
                else:
                    print(f"{inner_pad}{_format_basic(item)}")
            print(pad + "]")
    else:
        # Basic type: inline
        print(_format_basic(value))


def _is_three_numbers(value):
    return (
        isinstance(value, list)
        and len(value) == 3
        and all(isinstance(x, (int, float)) and not isinstance(x, bool) for x in value)
    )


def _format_basic(value):
    if isinstance(value, str):
        return f'"{value}"'
    if value is None:
        return "null"
    if isinstance(value, bool):
        return "true" if value else "false"
    return str(value)


