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

   configuration "release"
   defines { "NDEBUG" }
   flags { "Optimize" }
