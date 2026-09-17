#!/usr/bin/env bash
# Version: 3.1
# Date: 2026-09-21
# This bash script generates CMSIS-Compiler documentation
#
# Pre-requisites:
# - bash shell (for Windows: install git for Windows)
# - doxygen 1.18.0
# - linkchecker (can be skipped with -s)

set -o pipefail

# Set version of gen pack library
# For available versions see https://github.com/Open-CMSIS-Pack/gen-pack/tags.
# Use the tag name without the prefix "v", e.g., 0.7.0
REQUIRED_GEN_PACK_LIB="0.14.0"

DIRNAME=$(dirname "$(readlink -f "$0")")
GENDIR=../html
REQ_DXY_VERSION="1.18.0"

RUN_LINKCHECKER=1
COMPONENTS=()

function usage() {
  echo "Usage: $(basename "$0") [-h] [-s] [-c <comp>]"
  echo " -h,--help               Show usage"
  echo " -s,--no-linkcheck       Skip linkcheck"
  echo " -c,--component <comp>   Select component <comp> to generate documentation for. "
  echo "                         Can be given multiple times. Defaults to all components."
}

while [[ $# -gt 0 ]]; do
  case $1 in
    '-h'|'help')
      usage
      exit 1
    ;;
    '-s'|'--no-linkcheck')
      RUN_LINKCHECKER=0
    ;;
    '-c'|'--component')
      shift
      COMPONENTS+=("$1")
    ;;
    *)
      echo "Invalid command line argument: $1" >&2
      usage
      exit 1
    ;;
  esac
  shift # past argument
done

############ DO NOT EDIT BELOW ###########

# Set GEN_PACK_LIB_PATH to use a specific gen-pack library root
# ... instead of bootstrap based on REQUIRED_GEN_PACK_LIB
if [[ -f "${GEN_PACK_LIB_PATH}/gen-pack" ]]; then
  . "${GEN_PACK_LIB_PATH}/gen-pack"
else
  . <(curl -sL "https://raw.githubusercontent.com/Open-CMSIS-Pack/gen-pack/main/bootstrap")
fi

find_git
find_doxygen "${REQ_DXY_VERSION}"
[[ ${RUN_LINKCHECKER} != 0 ]] && find_linkchecker

if [ -z "${VERSION_FULL}" ]; then
  VERSION_FULL=$(git_describe "v")
fi

pushd "${DIRNAME}" > /dev/null || exit 1

# Generate documentation for one component.
#
# Arguments:
#   $1  Component name and Doxyfile basename.
#   $2  Directory containing the component Doxyfile template.
#   $3  Directory for the generated HTML documentation.
#   $4  Full project version. When empty, use PROJECT_NUMBER from the component
#       Doxyfile template, then fall back to the short Git commit hash.
#   $5  Revision history mode: "none", "release", or "development".
#
# Components excluded by the --component option are skipped.
function generate() {
  local component="$1"
  local sourceDir="$2"
  local outputDir="$3"
  local projectNumberFull="$4"
  local historyMode="$5"
  local requestedComponent
  local componentSelected=false

  # Honor the optional component selection made on the command line.
  if [[ ${#COMPONENTS[@]} == 0 ]]; then
    componentSelected=true
  else
    for requestedComponent in "${COMPONENTS[@]}"; do
      if [[ "${requestedComponent}" == "${component}" ]]; then
        componentSelected=true
        break
      fi
    done
  fi
  if [[ "${componentSelected}" != "true" ]]; then
    return 0
  fi

  pushd "${sourceDir}" > /dev/null || return 1

  # Extract the project name from the Doxyfile template.
  local projectName
  projectName=$(grep -E "PROJECT_NAME\s+=" "${component}.dxy.in" | sed -r -e 's/[^"]*"([^"]+)".*/\1/')

  if [ -z "${projectNumberFull}" ]; then
    # No project version was provided, try to extract it from the Doxyfile template.
    projectNumberFull=$(grep -E "PROJECT_NUMBER\s+=" "${component}.dxy.in" | sed -r -e 's/[^"]*"[^0-9]*(([0-9]+\.[0-9]+(\.[0-9]+)?(-.+)?)?)".*/\1/')
  fi
  if [ -z "${projectNumberFull}" ]; then
    # No project version in the Doxyfile template, fall back to the short Git commit hash.
    projectNumberFull="$(git rev-parse --short HEAD)"
  fi
  # Extract the short project version (without any build metadata).
  local projectNumber
  projectNumber="${projectNumberFull%+*}"

  # Get the current date and year for the documentation footer.
  local datetime
  local year
  datetime=$(date -u +'%a %b %e %Y %H:%M:%S')
  year=$(date -u +'%Y')

  # Instantiate the component Doxyfile.
  sed -e "s/{projectNumber}/${projectNumber}/" "${component}.dxy.in" > "${component}.dxy"

  # Ensure the component output directory exists.
  mkdir -p "${outputDir}"

  # Generate the requested revision history, if any.
  case "${historyMode}" in
    'none')
      ;;
    'release')
      git_changelog -f html -p "v" > src/revision_history.txt
      ;;
    'development')
      git_changelog -f html -p "v" -d > src/revision_history.txt
      ;;
    *)
      echo "Invalid revision history mode for ${component}: ${historyMode}" >&2
      popd > /dev/null || return 1
      return 1
      ;;
  esac

  # Generate the component HTML documentation.
  echo_log "\"${UTILITY_DOXYGEN}\" \"${component}.dxy\""
  if ! "${UTILITY_DOXYGEN}" "${component}.dxy"; then
    popd > /dev/null || return 1
    return 1
  fi

  # Instantiate the footer metadata after Doxygen creates the HTML output.
  sed -e "s/{datetime}/${datetime}/" "${DIRNAME}/style_template/footer.js.in" \
    | sed -e "s/{year}/${year}/" \
    | sed -e "s/{projectName}/${projectName}/" \
    | sed -e "s/{projectNumber}/${projectNumber}/" \
    | sed -e "s/{projectNumberFull}/${projectNumberFull}/" \
    > "${outputDir}/footer.js"

  popd > /dev/null || return 1
}

# Generate all components, or only those selected with --component.
echo_log "Generating documentation ..."
generate "compiler" "." "${DIRNAME}/${GENDIR}" "${VERSION_FULL}" "release" || exit 1

# Validate generated links unless link checking was disabled.
[[ ${RUN_LINKCHECKER} != 0 ]] && check_links --timeout 120 "${DIRNAME}/../html/index.html" "${DIRNAME}"

popd > /dev/null || exit 1

exit 0
