/*
 * Copyright © 2020 Collabora, Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "config.h"

#include "weston-test-client-helper.h"
#include "weston-test-fixture-compositor.h"

static enum test_result_code
fixture_setup(struct weston_test_harness *harness)
{
	struct compositor_setup setup;

	compositor_setup_defaults(&setup);

	return weston_test_harness_execute_as_client(harness, &setup);
}
DECLARE_FIXTURE_SETUP(fixture_setup);

TEST(invalid_buffer_size)
{
	struct client *client;
	struct buffer *buffer;
	struct wl_surface *surface;
	pixman_color_t blue;
	int frame;

	color_rgb888(&blue, 0, 0, 255);

	client = create_client_and_test_surface(0, 0, 200, 200);
	assert(client);
	surface = client->surface->wl_surface;
	/* create buffer 100x100 and default scale is 1
	 * no error for first attach */
	buffer = create_shm_buffer_a8r8g8b8(client, 100, 100);
	fill_image_with_color(buffer->image, &blue);
	wl_surface_attach(surface, buffer->proxy, 0, 0);
	wl_surface_damage(surface, 0, 0, 200, 200);
	frame_callback_set(surface, &frame);
	wl_surface_commit(surface);
	frame_callback_wait_nofail(client, &frame);
	/* Now buffer 100x100 and set buffer scale to 3
	 * whose height and width are not integer
	 * multi/ple buffer scale error should be raised*/
	wl_surface_set_buffer_scale(client->surface->wl_surface, 3);
	wl_surface_attach(surface, buffer->proxy, 0, 0);

	expect_protocol_error(client, &wl_surface_interface,
			      WL_SURFACE_ERROR_INVALID_SIZE);

}
