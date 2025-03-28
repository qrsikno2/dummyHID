savedcmd_dummyhid.mod := printf '%s\n'   dummyhid.o | awk '!x[$$0]++ { print("./"$$0) }' > dummyhid.mod
