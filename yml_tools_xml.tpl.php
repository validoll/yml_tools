<?php
/**
 * @file
 * Theme implementation to display YML.
 *
 * Available variables:
 * - $name:
 * - $company:
 * - $date:
 * - $currency:
 * - $products:
 * - $categories:
 */

?>
<?php
echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" ?>
<!DOCTYPE yml_catalog SYSTEM "shops.dtd">
<yml_catalog date="<?php echo $date; ?>">
<shop>
  <name><?php echo $name; ?></name>
  <company><?php echo $company; ?></company>
  <url><?php echo $url; ?></url>

<currencies><currency id="<?php echo $currency; ?>" rate="1"/></currencies>
<categories>
<?php echo $categories; ?>
</categories>

<offers>
<?php echo $products; ?>
</offers>

</shop>
</yml_catalog>
