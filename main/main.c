/* The example of ESP-IDF
 *
 * This sample code is in the public domain.
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "img_converters.h"
#include "quirc.h"

#define IMAGE_WIDTH 512
#define IMAGE_HEIGHT 512
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// Helper functions to convert an RGB565 image to grayscale
typedef union {
	uint16_t val;
	struct {
		uint16_t b: 5;
		uint16_t g: 6;
		uint16_t r: 5;
	};
} rgb565_t;

static uint8_t rgb565_to_grayscale(const uint8_t *img)
{
	uint16_t *img_16 = (uint16_t *) img;
	rgb565_t rgb = {.val = __builtin_bswap16(*img_16)};
	uint16_t val = (rgb.r * 8 + rgb.g * 4 + rgb.b * 8) / 3;
	return (uint8_t) MIN(255, val);
}

static void rgb565_to_grayscale_buf(const uint8_t *src, uint8_t *dst, int qr_width, int qr_height)
{
	for (size_t y = 0; y < qr_height; y++) {
		for (size_t x = 0; x < qr_width; x++) {
			dst[y * qr_width + x] = rgb565_to_grayscale(&src[(y * qr_width + x) * 2]);
		}
	}
}

void qr_task(void *pvParameter)
{
	extern const uint8_t jpg_start[] asm("_binary_qr_code_jpg_start");
	extern const uint8_t jpg_end[] asm("_binary_qr_code_jpg_end");
	const uint32_t jpg_len = jpg_end - jpg_start;
	printf("jpg_len=%ld\n", jpg_len);

	// Read jpg file
	unsigned char* jpg_buffer = NULL;
	jpg_buffer = malloc(jpg_len);
	if (jpg_buffer == NULL) {
		perror("jpg_buffer malloc fail");
		abort();
	}
	memcpy(jpg_buffer, jpg_start, jpg_len);

	// Allocate rgb565 buffer
	uint8_t *rgb565_buffer = NULL;
	rgb565_buffer = malloc(IMAGE_WIDTH * IMAGE_HEIGHT * 2);
	if (rgb565_buffer == NULL) {
		perror("Failed to allocate rgb565 buffer");
		abort();
	}

	// Convert from jpeg to rgb565
	bool success = jpg2rgb565(jpg_buffer, jpg_len, rgb565_buffer, JPG_SCALE_NONE);
	printf("jpg2rgb565 success=%d\n", success);
	if (!success) {
		perror("Failed to jpg2rgb565");
		abort();
	}
	free(jpg_buffer);


	struct quirc *qr;

	qr = quirc_new();
	if (!qr) {
		perror("Failed to allocate memory");
		abort();
	}

	if (quirc_resize(qr, IMAGE_WIDTH, IMAGE_HEIGHT) < 0) {
		perror("Failed to allocate video memory");
		abort();
	}

	// Convert from rgb565 to grayscale
	uint8_t *qr_buf = quirc_begin(qr, NULL, NULL);
	rgb565_to_grayscale_buf(rgb565_buffer, qr_buf, IMAGE_WIDTH, IMAGE_HEIGHT);
	free(rgb565_buffer);

	// Process the frame. This step find the corners of the QR code (capstones)
	quirc_end(qr);
	int num_codes = quirc_count(qr);
	printf("QR num_codes=%d heap_caps_get_free_size=%d\n",
		num_codes, heap_caps_get_free_size(MALLOC_CAP_DEFAULT));

	// If a QR code was detected, try to decode it:
	for (int i = 0; i < num_codes; i++) {
		// Decode the raw data
		struct quirc_code code;
		quirc_extract(qr, i, &code);

#if 1
		// Decoding stage
		struct quirc_data qr_data;
		quirc_decode_error_t err;
		err = quirc_decode(&code, &qr_data);
		printf("quirc_decode err=%d\n", err);
		if (err == QUIRC_ERROR_DATA_ECC) {
			quirc_flip(&code);
			err = quirc_decode(&code, &qr_data);
		}
		if (err) {
			printf("DECODE FAILED: %s\n", quirc_strerror(err));
		} else {
			printf("qr_data.payload_len=%d\n", qr_data.payload_len);
			printf("qr_data.payload=[%.*s]\n", qr_data.payload_len, qr_data.payload);
		}
#endif
	} // end for

	quirc_destroy(qr);
	vTaskDelete(NULL);

}

void app_main()
{
	xTaskCreate(qr_task, "QRCODE", 1024*6, NULL, 2, NULL);
}

