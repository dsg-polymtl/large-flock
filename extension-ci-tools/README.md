# Extension CI Tools for DuckDB
This repository contains reusable components for building, testing and deploying DuckDB extensions.

DuckDB's [Extension Template](https://github.com/duckdb/extension-template/actions) and various DuckDB Extensions based on the template use this repository to deduplicate code for build configuration and easily update the extension repositories when changes occur to DuckDB's build system and/or CI.

## Versioning
| Extension-ci-tools Branch | DuckDB target version | Actively maintained? |
|---------------------------|-----------------------|----------------------|
| main                      | main                  | yes                  |
| v1.0.0                    | v1.0.0                | yes                  |
| v0.10.3                   | v0.10.3               | no                   |
| v0.10.2                   | v0.10.2               | no                   |
| v0.10.1                   | v0.10.1               | no                   |
| v0.10.0                   | v0.10.0               | no                   |

Each branch in this repository targets a specific version of DuckDB. Note that these branches will be continually updated to ensure the build environment is functional for that version of DuckDB.
Also note that at some point, support for versions will be dropped. Currently, we aim to support the latest 2 DuckDB versions, to allow extensions devs to transition to a new DuckDB version.
