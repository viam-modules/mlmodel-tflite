$ErrorActionPreference = "Stop"

Remove-Item -Recurse -Force build-conan -ErrorAction SilentlyContinue

conan build . `
      --output-folder=build-conan `
      --build=missing `
      -o "&:shared=False" `
      -s:h compiler.cppstd=17 `
      -c:h tools.microsoft:winsdk_version=10.0.17763.0 `
      -s:h compiler.runtime=static
