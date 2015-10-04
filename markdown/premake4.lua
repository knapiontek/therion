#!lua

solution "markdown"
   configurations { "debug", "release" }

   project "markdown"
   kind "ConsoleApp"
   language "C++"
   files { "**.h", "**.cpp" }

   configuration "debug"
   defines { "DEBUG" }
   flags { "Symbols" }
   buildoptions { "-std=c++11" }

   configuration "release"
   defines { "NDEBUG" }
   flags { "Optimize" }
