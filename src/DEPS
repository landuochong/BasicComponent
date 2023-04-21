gclient_gn_args = [
  'generate_location_tags',
]

vars = {
  # ninja CIPD package version
  # https://chrome-infra-packages.appspot.com/p/infra/3pp/tools/ninja
  'ninja_version': 'version:2@1.8.2.chromium.3',
}

deps = {
  'src/third_party/abseil-cpp':
    'https://github.com/abseil/abseil-cpp.git@c8a2f92586fe9b4e1aff049108f5db8064924d8e',
}

hooks = [  
]

recursedeps = []

# Define rules for which include paths are allowed in our source.
include_rules = [
  # These should eventually move out of here.
  # Abseil flags are allowed in tests and tools.
  "+absl/flags",
]


