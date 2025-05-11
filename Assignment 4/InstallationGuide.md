## Setup and Installation on Ubuntu

To install Flex and Yacc (or Bison, a compatible parser generator):

### Install Flex:

```sh
sudo apt-get update
sudo apt-get install flex
```

### Install Yacc/Bison:

You can install Bison, which is largely compatible with Yacc:

```sh
sudo apt-get install byacc
sudo apt-get install bison
sudo apt-get install bison++
sudo apt-get install byacc-j
```

**Note:** Some systems may have Yacc installed by default, but Bison is more widely used today.

### Verify Installation:

Check that Flex and Bison (or Yacc) are installed:

```sh
flex --version
yacc --version
```

### Manuals

- [Flex Manual](https://westes.github.io/flex/manual/)
- [Bison (Yacc) Manual](https://www.gnu.org/software/bison/manual/)
