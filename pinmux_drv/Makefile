include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk



PKG_NAME:=pinmux-drv
PKG_VERSION:=1.0


define KernelPackage/pinmux-drv
  SUBMENU:=jansion drivers
  TITLE:=pinmux-drv for control mt6788's gpio#21
  FILES:=$(PKG_BUILD_DIR)/pinmux-drv.ko
  #AUTOLOAD:=$(call AutoLoad, 21, pinmux-drv)
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/

endef


define Build/Compile

	$(MAKE) -C "$(LINUX_DIR)"\
			ARCH="$(LINUX_KARCH)"\
			CROSS_COMPILE="$(TARGET_CROSS)"\
			SUBDIRS="$(PKG_BUILD_DIR)"\
			EXTRA_CFLAGS="$(EXTRA_CFLAGS)"\
			$(EXTRA_KCONFIG)\
			modules

endef


$(eval $(call KernelPackage,pinmux-drv))


