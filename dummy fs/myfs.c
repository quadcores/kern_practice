#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

static struct dentry *aufs_mount(struct file_system_type *type, int flags,
				   char const *dev, void *data);
static int aufs_fill_sb(struct super_block *sb, void *data, int silent);
static void aufs_put_super(struct super_block *sb);

static struct file_system_type aufs_type = {
  .owner = THIS_MODULE,
  .name = "aufs",
  .mount = aufs_mount,
  .kill_sb = kill_block_super,
  .fs_flags = FS_REQUIRES_DEV, 
};

static struct super_operations const aufs_super_ops = {
  .put_super = aufs_put_super,
};

static int __init aufs_init(void)
{
  printk(KERN_INFO"aufs module loaded\n");  
  register_filesystem(&aufs_type);
  return 0;
}

static void __exit aufs_fini(void)
{
  unregister_filesystem(&aufs_type);
  printk(KERN_INFO"aufs module unloaded\n");
}

static struct dentry *aufs_mount(struct file_system_type *type, int flags,
				 char const *dev, void *data)
{
  struct dentry *const entry = mount_bdev(type, flags, dev,
					  data, aufs_fill_sb);
  if (IS_ERR(entry))
    printk(KERN_ALERT"aufs mounting failed\n");
  else
    printk(KERN_INFO"aufs mounted\n");
  return entry;
}

static int aufs_fill_sb(struct super_block *sb, void *data, int silent)
{
  struct inode *root = NULL;

  sb->s_magic = 312;
  sb->s_op = &aufs_super_ops;

  root = new_inode(sb);
  if (!root)
    {
      printk(KERN_ALERT"inode allocation failed\n");
      return -ENOMEM;
    }

  root->i_ino = 0;
  root->i_sb = sb;
  root->i_atime = root->i_mtime = root->i_ctime = CURRENT_TIME;
  inode_init_owner(root, NULL, S_IFDIR);

  sb->s_root = d_make_root(root);
  if (!sb->s_root)
    {
      printk(KERN_ALERT"root creation failed\n");
      return -ENOMEM;
    }

  return 0;
}

static void aufs_put_super(struct super_block *sb)
{
  printk(KERN_INFO"aufs super block destroyed\n");
}

module_init(aufs_init);
module_exit(aufs_fini);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kmu");
