#!/nix/store/jdi2v7ir1sr6vp7pc5x0nhb6lpcmg6xg-bash-4.4-p23/bin/bash
set -x
set -euo pipefail
in=build/zephyr/zephyr.hex
out=build/zephyr.zip
if [[ -f "in" ]]; then
  set -x
  /nix/store/fjmmha3m3p8hvyfn0wvghlyjqbndb9xg-nrfutil-6.1/bin/nrfutil pkg generate --hw-version 52 --sd-req=0x00 \
          --application "$in" \
          --application-version 1 "$out"
  /nix/store/fjmmha3m3p8hvyfn0wvghlyjqbndb9xg-nrfutil-6.1/bin/nrfutil dfu usb-serial -pkg "$out" -p "${1:-/dev/ttyACM0}"
fi

