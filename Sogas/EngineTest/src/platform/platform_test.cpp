
#include "pch.h"

#include <../include/platform/platform.h>

using namespace sogas::engine::platform;

TEST(PlatformTest, OpenWindowWithoutParameters)
{
    auto window = create_window();

    EXPECT_TRUE(window >= 0);
    EXPECT_FALSE(is_window_fullscreen(window));

    u32 w, h;
    get_window_size(window, w, h);
    EXPECT_EQ(w, 1920);
    EXPECT_EQ(h, 1080);

    remove_window(window);
}

TEST(PlatformTest, OpenWindowWithParameters)
{
    window_init_info window_info{"Sogas Engine Test", nullptr, nullptr, 0, 0, 1280, 720};
    auto             window = create_window(&window_info);

    EXPECT_TRUE(window >= 0);
    EXPECT_FALSE(is_window_fullscreen(window));

    u32 w, h;
    get_window_size(window, w, h);
    EXPECT_EQ(w, 1280);
    EXPECT_EQ(h, 720);

    remove_window(window);
}

TEST(PlatformTest, ResizeWindow)
{
    window_init_info window_info{"Sogas Engine Test", nullptr, nullptr, 0, 0, 1280, 720};
    auto             window = create_window(&window_info);

    EXPECT_TRUE(window >= 0);
    EXPECT_FALSE(is_window_fullscreen(window));

    u32 w, h;
    get_window_size(window, w, h);
    EXPECT_EQ(w, 1280);
    EXPECT_EQ(h, 720);

    resize_window(window, 800, 400);

    get_window_size(window, w, h);
    EXPECT_EQ(w, 800);
    EXPECT_EQ(h, 400);

    remove_window(window);
}

TEST(PlatformTest, SetFullscreenWindow)
{
    auto window = create_window();

    EXPECT_TRUE(window >= 0);
    EXPECT_FALSE(is_window_fullscreen(window));

    set_window_fullscreen(window, true);

    EXPECT_TRUE(is_window_fullscreen(window));

    remove_window(window);
}