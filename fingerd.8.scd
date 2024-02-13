fingerd(8)

# NAME

fingerd - a simple finger server

# SYNOPSIS

*fingerd* [-a _addr_] [-p _port_] [-g _group_] [-h] [-v]

# DESCRIPTION

*fingerd* runs a finger server on the specified host (default: 127.0.0.1) and
the specified port (default: 79). It responds with either a list of users or the
contents of the requested user's *~/.plan* file.

## OPTIONS

*-a* _addr_
	binds finger daemon to _addr_.
	default: 127.0.0.1

*-p* _port_
	listens on _port_.
	default: 79

*-g* _group_
	looks for the *~/.plan* file of users in _group_.
	default: finger

*-h*
	prints usage information, then exits.

*-v*
	prints version information to stderr, then exits.

# SEE ALSO

*finger*(1)

# AUTHOR

Bryce Vandegrift <bryce@brycevandegrift.xyz>
