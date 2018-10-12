file(REMOVE_RECURSE
  "../lib/libHPPG-v1-0-0.pdb"
  "../lib/libHPPG-v1-0-0.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/HPPG.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
