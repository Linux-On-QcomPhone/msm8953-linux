// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2026 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct oppo16027jdi_r63452 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data *supplies;
	struct gpio_desc *reset_gpio;
	struct backlight_device *bl;
};

static const struct regulator_bulk_data oppo16027jdi_r63452_supplies[] = {
	{ .supply = "vsp" },
	{ .supply = "vsn" },
};

static inline
struct oppo16027jdi_r63452 *to_oppo16027jdi_r63452(struct drm_panel *panel)
{
	return container_of(panel, struct oppo16027jdi_r63452, panel);
}

static void oppo16027jdi_r63452_reset(struct oppo16027jdi_r63452 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(15000, 16000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(2000, 3000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(15000, 16000);
}

static int oppo16027jdi_r63452_on(struct oppo16027jdi_r63452 *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_tear_on_multi(&dsi_ctx, MIPI_DSI_DCS_TEAR_MODE_VBLANK);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb0, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xe9, 0x40, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd6, 0x01);
	/*
	 * This panel expects DCS 0x51 (set display brightness) with a single
	 * parameter (0..255). Sending the generic two-byte form leaves the
	 * panel at the wrong brightness and makes dimming impossible.
	 */
	mipi_dsi_dcs_write_var_seq_multi(&dsi_ctx,
					 MIPI_DCS_SET_DISPLAY_BRIGHTNESS,
					 ctx->bl ? ctx->bl->props.brightness : 0xff);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_CONTROL_DISPLAY,
				     0x24);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_POWER_SAVE, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_SET_CABC_MIN_BRIGHTNESS,
				     0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb9,
				     0x8f, 0x4d, 0x13, 0x20, 0x02, 0x30, 0x30);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xce,
				     0x11, 0x40, 0x49, 0x53, 0x59, 0x5e, 0x63,
				     0x68, 0x6e, 0x74, 0x7e, 0x8a, 0x98, 0xa8,
				     0xbb, 0xd0, 0xff, 0x04, 0x00, 0x04, 0x04,
				     0x42, 0x00, 0x69, 0x5a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x11, 0x00);
	mipi_dsi_msleep(&dsi_ctx, 120);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x29, 0x00);
	mipi_dsi_msleep(&dsi_ctx, 20);

	return dsi_ctx.accum_err;
}

static int oppo16027jdi_r63452_off(struct oppo16027jdi_r63452 *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 20);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);

	return dsi_ctx.accum_err;
}

static int oppo16027jdi_r63452_prepare(struct drm_panel *panel)
{
	struct oppo16027jdi_r63452 *ctx = to_oppo16027jdi_r63452(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(oppo16027jdi_r63452_supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	oppo16027jdi_r63452_reset(ctx);

	ret = oppo16027jdi_r63452_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(oppo16027jdi_r63452_supplies), ctx->supplies);
		return ret;
	}

	return 0;
}

static int oppo16027jdi_r63452_unprepare(struct drm_panel *panel)
{
	struct oppo16027jdi_r63452 *ctx = to_oppo16027jdi_r63452(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = oppo16027jdi_r63452_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(oppo16027jdi_r63452_supplies), ctx->supplies);

	return 0;
}

static const struct drm_display_mode oppo16027jdi_r63452_mode = {
	.clock = (1080 + 100 + 2 + 94) * (1920 + 8 + 4 + 20) * 60 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 100,
	.hsync_end = 1080 + 100 + 2,
	.htotal = 1080 + 100 + 2 + 94,
	.vdisplay = 1920,
	.vsync_start = 1920 + 8,
	.vsync_end = 1920 + 8 + 4,
	.vtotal = 1920 + 8 + 4 + 20,
	.width_mm = 68,
	.height_mm = 122,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int oppo16027jdi_r63452_get_modes(struct drm_panel *panel,
					 struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &oppo16027jdi_r63452_mode);
}


#define OPPO16027JDI_R63452_MAX_BRIGHTNESS	255

static int oppo16027jdi_r63452_bl_update_status(struct backlight_device *bl)
{
	struct oppo16027jdi_r63452 *ctx = bl_get_data(bl);
	u8 brightness = backlight_get_brightness(bl);
	int ret;

	ctx->dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	/* DCS 0x51 takes exactly one parameter on this panel. */
	ret = mipi_dsi_dcs_write(ctx->dsi, MIPI_DCS_SET_DISPLAY_BRIGHTNESS,
				 &brightness, sizeof(brightness));
	if (ret < 0)
		return ret;

	return 0;
}

static const struct backlight_ops oppo16027jdi_r63452_bl_ops = {
	.update_status = oppo16027jdi_r63452_bl_update_status,
};

static const struct drm_panel_funcs oppo16027jdi_r63452_panel_funcs = {
	.prepare = oppo16027jdi_r63452_prepare,
	.unprepare = oppo16027jdi_r63452_unprepare,
	.get_modes = oppo16027jdi_r63452_get_modes,
};

/*
 * Debug helper: send a raw DCS command to the panel.
 *   echo "51 0F"    > dcs_write   -> DCS 0x51 with payload 0x0F
 *   echo "53 2C"    > dcs_write   -> DCS 0x53 with payload 0x2C
 * Used to probe panel-internal brightness/dimming controls while porting.
 */
static ssize_t dcs_write_store(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count)
{
	struct oppo16027jdi_r63452 *ctx = dev_get_drvdata(dev);
	u8 cmd[32];
	int n = 0;
	const char *p = buf;

	while (n < (int)sizeof(cmd)) {
		unsigned int v;
		char *end;

		v = simple_strtoul(p, &end, 16);
		if (end == p)
			break;
		cmd[n++] = v & 0xff;
		p = end;
		while (*p == ' ' || *p == '\t' || *p == ',')
			p++;
		if (*p == '\0' || *p == '\n')
			break;
	}

	if (n < 1)
		return -EINVAL;

	ctx->dsi->mode_flags |= MIPI_DSI_MODE_LPM;
	mipi_dsi_dcs_write(ctx->dsi, cmd[0], &cmd[1], n - 1);
	dev_info(dev, "dcs_write: cmd 0x%02x len %d applied\n", cmd[0], n - 1);

	return count;
}
static DEVICE_ATTR_WO(dcs_write);

static struct attribute *oppo16027jdi_r63452_attrs[] = {
	&dev_attr_dcs_write.attr,
	NULL,
};

static const struct attribute_group oppo16027jdi_r63452_attr_group = {
	.attrs = oppo16027jdi_r63452_attrs,
};

static int oppo16027jdi_r63452_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct oppo16027jdi_r63452 *ctx;
	int ret;

	ctx = devm_drm_panel_alloc(dev, struct oppo16027jdi_r63452, panel,
				   &oppo16027jdi_r63452_panel_funcs,
				   DRM_MODE_CONNECTOR_DSI);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ret = devm_regulator_bulk_get_const(dev,
					    ARRAY_SIZE(oppo16027jdi_r63452_supplies),
					    oppo16027jdi_r63452_supplies,
					    &ctx->supplies);
	if (ret < 0)
		return ret;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	ret = devm_device_add_group(dev, &oppo16027jdi_r63452_attr_group);
	if (ret)
		return dev_err_probe(dev, ret, "Failed to add sysfs groups\n");

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO_BURST | MIPI_DSI_MODE_VIDEO_HSE |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;

	ctx->panel.prepare_prev_first = true;

	{
		struct backlight_properties props = {
			.type = BACKLIGHT_RAW,
			.max_brightness = OPPO16027JDI_R63452_MAX_BRIGHTNESS,
			.brightness = OPPO16027JDI_R63452_MAX_BRIGHTNESS,
		};

		ctx->bl = devm_backlight_device_register(dev, "backlight", dev,
							 ctx,
							 &oppo16027jdi_r63452_bl_ops,
							 &props);
		if (IS_ERR(ctx->bl))
			return dev_err_probe(dev, PTR_ERR(ctx->bl),
					     "Failed to register backlight\n");

		ctx->panel.backlight = ctx->bl;
	}

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
	}

	return 0;
}

static void oppo16027jdi_r63452_remove(struct mipi_dsi_device *dsi)
{
	struct oppo16027jdi_r63452 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id oppo16027jdi_r63452_of_match[] = {
	{ .compatible = "oppo,r63452" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, oppo16027jdi_r63452_of_match);

static struct mipi_dsi_driver oppo16027jdi_r63452_driver = {
	.probe = oppo16027jdi_r63452_probe,
	.remove = oppo16027jdi_r63452_remove,
	.driver = {
		.name = "panel-oppo16027jdi-r63452",
		.of_match_table = oppo16027jdi_r63452_of_match,
	},
};
module_mipi_dsi_driver(oppo16027jdi_r63452_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for oppo16027jdi r63452 1080p cmd mode dsi panel");
MODULE_LICENSE("GPL");
