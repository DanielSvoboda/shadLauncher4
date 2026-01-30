#!/usr/bin/env bash
set -e

# Always operate from the repository root
cd "$GITHUB_WORKSPACE"

# Ensure we have full history and tags
git fetch --tags --force

# Try to get the latest tag
if git describe --tags --abbrev=0 >/dev/null 2>&1; then
  latest_tag=$(git describe --tags --abbrev=0)
  commit_count=$(git rev-list "${latest_tag}..HEAD" --count)
else
  latest_tag="0.0.0"
  commit_count=$(git rev-list HEAD --count)
fi

# Short hash and date
short_hash=$(git rev-parse --short HEAD)
date_str=$(date +%Y%m%d)

# Combine into version string
# Example: v0.0.1 build 20 20250130-ab12cd3
version="v${latest_tag} build ${commit_count} ${date_str}-${short_hash}"

# Save to version.txt at repo root
echo "$version" > "$GITHUB_WORKSPACE/version.txt"

# Debug output
echo "Computed version: $version"
