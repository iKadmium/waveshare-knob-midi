FROM espressif/idf-rust:esp32s3_latest

# Ensure the 'esp' user has the correct permissions
USER esp
WORKDIR /home/esp

# Source the ESP environment variables automatically on shell login
RUN echo "source /home/esp/export-esp.sh" >> ~/.bashrc
