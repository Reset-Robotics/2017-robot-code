file(REMOVE_RECURSE
  "circle_detect.pdb"
  "circle_detect"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/circle_detect.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
