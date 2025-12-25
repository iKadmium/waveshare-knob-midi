set(PROJECT_INCLUDE_DIR "${COMPONENT_DIR}/../../include")

idf_component_register(
  SRCS "i2c_bsp.c"
  REQUIRES driver
  INCLUDE_DIRS "./" "${PROJECT_INCLUDE_DIR}")
