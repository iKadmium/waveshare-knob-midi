idf_component_register(
  SRCS "blemidi.c"
  REQUIRES bt
  PRIV_REQUIRES nvs_flash console
  INCLUDE_DIRS "include")
