file(REMOVE_RECURSE
  "../bin/HPPG-v1-0-0.pdb"
  "../bin/HPPG-v1-0-0"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/HPPG-bin.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
