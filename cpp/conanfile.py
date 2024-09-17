import os
from io import StringIO

from conan import ConanFile
from conan.errors import ConanException
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.build import can_run

class viamTfliteCpu(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("viam-cpp-sdk/0.0.10")
        self.requires("tensorflow-lite/2.15.0")
        self.requires("abseil/20240116.2", override=True)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def layout(self):
        cmake_layout(self, src_folder=".")

    def test(self):
        if can_run(self):
            cmd = os.path.join(self.cpp.build.bindir, "tflite_module")
            stderr = StringIO()
            self.run(cmd, env='conanrun', stderr=stderr, ignore_errors=True)
            if "main failed with exception:" not in stderr.getvalue():
                raise ConanException("Unexpected error output from test")
