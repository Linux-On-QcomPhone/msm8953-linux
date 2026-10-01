// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2026 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct otm1901a_dj_otp {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data *supplies;
	struct gpio_desc *reset_gpio;
};

static const struct regulator_bulk_data otm1901a_dj_otp_supplies[] = {
	{ .supply = "vsp" },
	{ .supply = "vsn" },
};

static inline
struct otm1901a_dj_otp *to_otm1901a_dj_otp(struct drm_panel *panel)
{
	return container_of(panel, struct otm1901a_dj_otp, panel);
}

static void otm1901a_dj_otp_reset(struct otm1901a_dj_otp *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
}

static int otm1901a_dj_otp_on(struct otm1901a_dj_otp *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff, 0x19, 0x01, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff, 0x19, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x35, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc4, 0x13);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff, 0xff, 0xff, 0xff);
	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x99, 0x11, 0x11);

	return dsi_ctx.accum_err;
}

static int otm1901a_dj_otp_off(struct otm1901a_dj_otp *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x99, 0x95, 0x27);
	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 50);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xf7,
					 0x5a, 0xa5, 0x19, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x99, 0x11, 0x11);

	return dsi_ctx.accum_err;
}

static int otm1901a_dj_otp_prepare(struct drm_panel *panel)
{
	struct otm1901a_dj_otp *ctx = to_otm1901a_dj_otp(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(otm1901a_dj_otp_supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	otm1901a_dj_otp_reset(ctx);

	ret = otm1901a_dj_otp_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(otm1901a_dj_otp_supplies), ctx->supplies);
		return ret;
	}

	return 0;
}

static int otm1901a_dj_otp_unprepare(struct drm_panel *panel)
{
	struct otm1901a_dj_otp *ctx = to_otm1901a_dj_otp(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = otm1901a_dj_otp_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(otm1901a_dj_otp_supplies), ctx->supplies);

	return 0;
}

static const struct drm_display_mode otm1901a_dj_otp_mode = {
	.clock = (1080 + 320 + 4 + 32) * (1920 + 14 + 2 + 9) * 59 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 320,
	.hsync_end = 1080 + 320 + 4,
	.htotal = 1080 + 320 + 4 + 32,
	.vdisplay = 1920,
	.vsync_start = 1920 + 14,
	.vsync_end = 1920 + 14 + 2,
	.vtotal = 1920 + 14 + 2 + 9,
	.width_mm = 68,
	.height_mm = 120,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int otm1901a_dj_otp_get_modes(struct drm_panel *panel,
				     struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &otm1901a_dj_otp_mode);
}

static const struct drm_panel_funcs otm1901a_dj_otp_panel_funcs = {
	.prepare = otm1901a_dj_otp_prepare,
	.unprepare = otm1901a_dj_otp_unprepare,
	.get_modes = otm1901a_dj_otp_get_modes,
};

static int otm1901a_dj_otp_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct otm1901a_dj_otp *ctx;
	int ret;

	ctx = devm_drm_panel_alloc(dev, struct otm1901a_dj_otp, panel,
				   &otm1901a_dj_otp_panel_funcs,
				   DRM_MODE_CONNECTOR_DSI);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ret = devm_regulator_bulk_get_const(dev,
					    ARRAY_SIZE(otm1901a_dj_otp_supplies),
					    otm1901a_dj_otp_supplies,
					    &ctx->supplies);
	if (ret < 0)
		return ret;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS |
			  MIPI_DSI_MODE_VIDEO_NO_HFP | MIPI_DSI_MODE_LPM;

	ctx->panel.prepare_prev_first = true;

	ret = drm_panel_of_backlight(&ctx->panel);
	if (ret)
		return dev_err_probe(dev, ret, "Failed to get backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
	}

	return 0;
}

static void otm1901a_dj_otp_remove(struct mipi_dsi_device *dsi)
{
	struct otm1901a_dj_otp *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id otm1901a_dj_otp_of_match[] = {
	{ .compatible = "mdss,otm1901a-dj-otp" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, otm1901a_dj_otp_of_match);

static struct mipi_dsi_driver otm1901a_dj_otp_driver = {
	.probe = otm1901a_dj_otp_probe,
	.remove = otm1901a_dj_otp_remove,
	.driver = {
		.name = "panel-otm1901a-dj-otp",
		.of_match_table = otm1901a_dj_otp_of_match,
	},
};
module_mipi_dsi_driver(otm1901a_dj_otp_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for otm1901a_1080p_video_DJ_otp");
MODULE_LICENSE("GPL");
