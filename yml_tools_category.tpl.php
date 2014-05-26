<?php
/**
 * @file
 * Theme implementation to display single category.
 *
 * Available variables:
 * - $category: taxonomy term object.
 */

 ?>
  <category id="<?php echo $category->tid; ?>"<?php if(isset($category->parent)): ?> parentId="<?php echo $category->parent; ?>"<?php endif; ?>><?php echo $category->name; ?></category>
