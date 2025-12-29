idf_component_register(
  SRCS "lcd_bl_pwm_bsp.c"
  PRIV_REQUIRES driver main esp_driver_ledc
  INCLUDE_DIRS "./")
