# finger

A simple, small, and portable finger client and server.

## About

A simple finger client and server written in C89 that can be built on
almost any POSIX 200809 or X/Open 700 system. In order to build documentation
[scdoc](https://sr.ht/~sircmpwn/scdoc/) is needed.

## Usage

### finger

To query a finger server just run:

``` sh
finger [user@]hostname
```

where `hostname` is the hostname of the system and `user` (which is optional)
is the name of the user.

### fingerd

To start the finger server just run (as root):

``` sh
fingerd
```

By default, fingerd binds to `127.0.0.1` (a.k.a. localhost) and to
port `79` which is the default finger port. It will look for any users
within the `finger` group.
