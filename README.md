# complete
A command-line filename completion

## Install
1. Compile:
`make`
2. Add the binary to `PATH`:
`export PATH=$PATH:/path/to/complete/bin`
3. Add the zsh plugin:
`source /path/to/complete/completion.zsh`

## Usage
1. Hit `TAB` key to complete the filename.

## Frecency
[Frecency Algorithm by Mozilla](https://developer.mozilla.org/en-US/docs/Mozilla/Tech/Places/Frecency_algorithm) is a combination of frequency and recency.

When a file is chosen, it gets points.
The points of files decay over time.
