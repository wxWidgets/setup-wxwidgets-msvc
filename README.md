# Set up wxWidgets for MSVC

This GitHub Action builds [wxWidgets](https://www.wxwidgets.org/) libraries
with MSVC on Windows runners and sets `WXWIN` environment variable to point to
them. This is all that the projects including `$(WXWIN)\wxwidgets.props`
property sheet, as recommended in wxWidgets documentation, need, so they can be
built with MSBuild in the subsequent steps.

As building wxWidgets takes much longer than building a typical application
using it, the build results are cached and reused in the subsequent runs, so
the libraries are only rebuilt when something affecting them changes.


## Usage

```yaml
jobs:
  build:
    runs-on: windows-2025
    steps:
      - uses: actions/checkout@v7

      - name: Set up wxWidgets
        uses: wxWidgets/setup-wxwidgets-msvc@v1
        with:
          ref: v3.3.4
          configuration: Release
          platform: x64

      - name: Set up MSBuild
        uses: microsoft/setup-msbuild@v3

      - name: Build
        run: msbuild -property:Configuration=Release -property:Platform=x64 MyApp.sln
```

Note that MSBuild steps should use `pwsh` (the default shell on Windows) or
`cmd`: when using `bash`, `msbuild` can't be found and `msbuild.exe` must be
used instead.


## Inputs

| Name | Default | Description |
| --- | --- | --- |
| `ref` | (required) | wxWidgets commit SHA, tag or branch to build. |
| `repository` | `wxWidgets/wxWidgets` | GitHub repository to get wxWidgets from. |
| `configuration` | `Release` | `wx_vc17.sln` configuration, e.g. `Debug` or `Release`. |
| `platform` | `x64` | `wx_vc17.sln` platform, e.g. `x64` or `Win32`. |
| `toolset` | latest | MSVC platform toolset, e.g. `v143`. |
| `cxx-standard` | compiler default | C++ standard for building wxWidgets: `14`, `17`, `20` or `latest`. |
| `projects` | all | Semicolon-separated list of `wx_vc17.sln` projects to build, e.g. `core;html`. |
| `submodules` | see below | Space-separated list of wxWidgets submodules to check out. |
| `extra-paths` | none | Newline-separated list of additional paths to cache, relative to wxWidgets directory. |
| `path` | `../wxWidgets` | Directory to put wxWidgets in, outside of the workspace by default. |
| `cache-version` | `1` | Change this to ignore all the previously cached builds. |

### `ref`

Using a release tag or a commit SHA is strongly recommended: a branch name is
resolved to its current tip commit, so wxWidgets is rebuilt every time the
branch changes.

### `toolset`

By default the toolset of the latest installed Visual Studio version is used.
If the project using wxWidgets specifies an older `PlatformToolset`, the same
toolset **must** be specified here, as the libraries built by a newer toolset
can't be linked using an older one.

### `cxx-standard`

This only affects wxWidgets itself and not the projects using it, which must
choose the C++ standard to use themselves. It is, however, recommended to use
the same standard for both wxWidgets and the projects using it to minimize the
risk of incompatibilities, so if the application uses e.g. `-std:c++20`,
`cxx-standard: 20` should be used with this action.

### `projects`

When not building everything, the application must exclude the libraries which
are not built from linking by predefining the corresponding `wxNO_XXX_LIB`
symbols, e.g. building just `core` (which also builds `base` and the
third-party libraries it uses) requires defining `wxNO_NET_LIB`,
`wxNO_XML_LIB`, `wxNO_HTML_LIB` and so on for all the other libraries.

### `submodules`

The default value is enough to build everything except wxSTC, which also needs
`src/stc/scintilla` and `src/stc/lexilla` submodules. The submodules not
existing in the version being built are skipped, so the default value can also
be used with older wxWidgets versions.

### `extra-paths`

Only the headers, libraries and property sheets are cached, so the cached
wxWidgets directory can't be used to build anything else, e.g. the samples.
If the application needs any other files, they must be specified here, e.g. a
project using Expat bundled with wxWidgets should use

```yaml
          extra-paths: |
            src/expat/expat/lib/*.h
```

to ensure that the Expat headers are available when using cached wxWidgets as
well.


## Outputs

| Name | Description |
| --- | --- |
| `wxwin` | Full path to wxWidgets directory, also available as `WXWIN` environment variable. |
| `cache-hit` | `true` if wxWidgets was restored from the cache. |


## Caching

The cache key includes the wxWidgets commit SHA, the major Visual Studio
version and the values of all the inputs affecting the build, so changing any
of them results in rebuilding the libraries. As usual with GitHub Actions
caches, the cache entries created on the default branch are available to all
the other branches and pull requests, but not vice versa.
