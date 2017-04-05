BassBoost- a variable system load
===================
`version 1.0`

```
I quickly needed something to test the boundaries of different container solutions, this program tries to (ab)use most system resources to provoke a force-kill or crash for testing purposes.

It doesn't do anything by default - the CLI opts are required to trigger the features.
```

# Features:
- RAM load generator
- optional CPU hogging

# ToDo:
- Variable fork bomb
- I/O load generator

# USAGE:
bassboost  [-q] [-c <int>] [-m <int>] [--] [--version] [-h]

# Where:
   -q,  --quiet
     Don't print anything

   -c <int>,  --cpu <int>
     Amount of cpu hog threads to spawn

   -m <int>,  --memory <int>
     Amount of ram load (in MiB) to generate

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.