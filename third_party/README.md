# Third-party libraries

cppio depends on several third-party libraries, their source code is available
(usually as a git submodule) in this directory.

## Guidelines on updating submodules

- IMPORTANT: whenever possible, try to only update to a stable release of a library (= not to master / random commit). Depending on unreleased revisions
  makes cppio installation harder for users, as it forces them to always build the dependency from source and prevents them from using more
  convenient installation channels (linux packages, package managers etc.)