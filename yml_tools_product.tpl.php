  <offer id="<?php echo $product->model ?>" available="true">
    <url><?php echo $product->url ?></url>
    <?php /* <vendorCode> echo $node -> model </vendorCode> */?>
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
