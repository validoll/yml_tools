<?php
/**
 * @file
 * Theme implementation to display single product.
 *
 * Available variables:
 * - $product: custom product object. Contain model, url, price, category,
 *               image, title, description properties
 * - $currency:
 */

 ?>
  <offer id="<?php echo $product->model ?>" available="true">
    <url><?php echo $product->url ?></url>
    <?php /* <vendorCode> echo $product->model; </vendorCode> */?>
    <price><?php echo $product->price ?></price>
    <currencyId><?php echo $currency; ?></currencyId>
    <categoryId><?php echo $product->category ?></categoryId>
    <?php if (!empty($product->image)): ?>
    <picture><?php echo $product->image; ?></picture>
    <?php endif; ?>
    <delivery><?php echo variable_get('yml_export_delivery', 'true'); ?></delivery>
    <name><?php echo $product->title ?></name>
    <description><?php echo $product->description; ?></description>
    <sales_notes></sales_notes>
  </offer>
