#!/bin/zsh

FN_COMPLETE="complete"

DIR_DELIM="/"
NL="
"

zle -N fnc
fnc() {
  fnc_prev_keymap=${KEYMAP:q}
  zle -K fnc
  fnc-complete
}

zle -N fnc-complete
fnc-complete() {
  local tokens

  tokens=(${(z)LBUFFER})
  if [[ $LBUFFER[-1] = ' ' ]]; then
    tokens+=("")
  fi

  fnc_arg=$(pwd)/${tokens[-1]}
  fnc_menu=("${(@f)$($FN_COMPLETE complete $fnc_arg)}")
  if [[ $fnc_menu[1] = "" ]]; then
    fnc_menu=()
  fi
  (( fnc_select = 0 ))

  fnc-next
}

zle -N fnc-next
fnc-next() {
  if (( ${#fnc_menu} > 0 )); then
    if (( fnc_select > 0 )); then
      LBUFFER=${LBUFFER:0:-1}
    fi
    if (( fnc_select == ${#fnc_menu} )); then
      (( fnc_select = 1 ))
    else
      (( fnc_select++ ))
    fi

    while [[ $LBUFFER[-1] != ' ' ]] && [[ $LBUFFER[-1] != '/' ]]; do
      LBUFFER=${LBUFFER:0:-1}
    done
    LBUFFER+=$fnc_menu[$fnc_select]
  fi

  if (( ${#fnc_menu} > 1 )); then
    POSTDISPLAY=$NL$fnc_menu
  else
    fnc-exit
  fi
}

zle -N fnc-exit-and-type
fnc-exit-and-type() {
  LBUFFER+=$KEYS
  fnc-exit
}

zle -N fnc-exit
fnc-exit() {
  $FN_COMPLETE choose $fnc_arg $fnc_menu[$fnc_select]
  POSTDISPLAY=
  zle -K $fnc_prev_keymap
}

# bind TAB to the entry point
bindkey '^I' fnc
bindkey -N fnc

# bind all keys to the fallback
bindkey -R -M fnc '\000-\377' fnc-exit

# bind printable keys
bindkey -R -M fnc '\x20-\x7E' fnc-exit-and-type

bindkey -M fnc '^I' fnc-next
