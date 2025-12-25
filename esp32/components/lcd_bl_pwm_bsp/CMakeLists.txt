set(PROJECT_INCLUDE_DIR "${COMPONENT_DIR}/../../include")

idf_component_register(
  SRCS "lcd_bl_pwm_bsp.c"
  REQUIRES driver                 
  INCLUDE_DIRS "./" "${PROJECT_INCLUDE_DIR}")
