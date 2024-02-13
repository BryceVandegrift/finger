finger(1)

# NAME

finger - a simple finger client

# SYNOPSIS

*finger* [_user_@]_host_ [-h] [-v]

# DESCRIPTION

*finger* is a simple finger client that will connect to the given _host_
(default: 127.0.0.1) and retrieve information about the given server or the
_user_ if specified.

## OPTIONS

*-h*
	prints usage information, then exits.

*-v*
	prints version information to stderr, then exits.

# SEE ALSO

*fingerd*(8)

# AUTHOR

Bryce Vandegrift <bryce@brycevandegrift.xyz>
