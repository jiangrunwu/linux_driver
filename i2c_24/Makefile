include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk



PKG_NAME:=i2c_24-cdrv
PKG_VERSION:=1.0


define KernelPackage/i2c_24-cdrv
  SUBMENU:=Dfrobot drivers
  TITLE:=i2c_24-cdrv for control mt6788's #16
  FILES:=$(PKG_BUILD_DIR)/i2c_24-cdrv.ko
  DEPENDS+=kmod-i2c-core
  #AUTOLOAD:=$(call AutoLoad, 21, i2c_24-cdrv)
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


$(eval $(call KernelPackage,i2c_24-cdrv))


