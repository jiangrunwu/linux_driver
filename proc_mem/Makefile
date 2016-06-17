include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk



PKG_NAME:=proc_mem-cdrv
PKG_VERSION:=1.0


define KernelPackage/proc_mem-cdrv
  SUBMENU:=Dfrobot drivers
  TITLE:=proc_mem-cdrv for control mt6788's #16
  FILES:=$(PKG_BUILD_DIR)/proc_mem-cdrv.ko
  #DEPENDS+=kmod-i2c-core
  #AUTOLOAD:=$(call AutoLoad, 21, proc_mem-cdrv)
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


$(eval $(call KernelPackage,proc_mem-cdrv))


