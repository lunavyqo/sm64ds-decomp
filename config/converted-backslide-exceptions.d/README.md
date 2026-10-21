# CONVERTED backslide log

One JSON file per identity that left the CONVERTED set. The gate does not read
these files to decide pass or fail. They are the record of why a removal was
allowed: `python tools/tiers_ratchet.py --update --reason "..."` writes them,
and it is the only writer.

Do not append to a shared log, and do not edit a file some other change added.
A new removal is a new file. The same path with the same reason is the same
file, so recording it twice changes nothing. A second reason for the same path
is a second file.

The filename is the source path plus a twelve-digit SHA-256 of the row. A `#`
in a `path#symbol` identity is written as `+`.
