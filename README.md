[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.7012895.svg)](https://doi.org/10.5281/zenodo.7012895)

# WBMsed

WBMsed is a distributed global sediment flux and water discharge model
that extends the [WBMplus (WBM/WTM) model](https://wsag.unh.edu/wbm.html)
by introducing a riverine sediment flux component based on the BQART and Psi models.

More detail on WBMsed can be found in
[Cohen *et al.* (2013)](http://dx.doi.org/10.1016/j.cageo.2011.08.011)
and at Prof. Cohen's research website,
[Global Sediment and Water Discharge Modeling](https://sdml.ua.edu/models/global-sediment-flux-modeling/).

## Use

WBMsed can be built and installed on Linux and macOS.
It requires an installation of the
[Global Hydrological Archive and Analysis System](https://wsag.unh.edu/toolsTop.html) (GHAAS),
with the `GHAASDIR` environment variable set to its install location.
The build system is CMake,
and an [install script](./install.sh) is provided.

As an alternative to building WBMsed from source,
it can be run through Docker.
Pull [the latest version of WBMsed](https://hub.docker.com/r/csdms/wbmsed) from Docker Hub with:
```sh
docker pull csdms/wbmsed
```

Clone this GitHub repository,
or simply download the Docker Compose file, [`compose.yaml`](./compose.yaml), from it.
From the directory that contains the Docker Compose file,
start a container:
```sh
docker compose run -it csdms/wbmsed
```
The local current working directory is mounted to the container at `/opt/wbmsed`,
allowing interactive shell access to WBMsed and its run scripts in the container,
as well as any input data available locally.

## Contact information

See the [WBMsed](https://csdms.colorado.edu/wiki/Model:WBMsed) page
in the [CSDMS Model Repository](https://csdms.colorado.edu/wiki/Model_download_portal).
