<?php
/**
 * @file
 * The code processing products and etc in the YML Tools module.
 */

interface YmlStoreTypeInterface {
  /**
   * Get store curency.
   */
  public function getCurrency();

  /**
   * Get price for single product.
   *
   * @param object $product
   *   Product node.
   *
   * @return float
   *   Product price
   */
  public function getProductPrice($product);

  /**
   * Get image for single product.
   *
   * @param object $product
   *   Product node
   *
   * @return string
   *   Product image url
   */
  public function getProductImage($product);

  /**
   * Get SKU for single product.
   *
   * @param object $product
   *   Product node
   *
   * @return string
   *   Product SKU
   */
  public function getProductSKU($product);
}

/**
 * Abstract class for store type
 */
abstract class YmlStoreType {
  protected $type;

  /**
   * Return store type.
   */
  public function getType() {
    return $this->type;
  }

  /**
   * Return node type for export.
   */
  public function getNodeTypes() {
    $types = array();
    $types_tmp = variable_get('yml_tools_types', array('product' => 'product'));
    foreach ($types_tmp as $type) {
      if (!empty($type)) {
        $types[$type] = $type;
      }
    }
    return $types;
  }

  /**
   * Get store categories.
   */
  public function getCategories() {
    $categories = taxonomy_get_tree(variable_get('yml_tools_vid', ''));
    return $categories;
  }

  /**
   * Get store products.
   */
  public function getProducts() {
    $products = array();
    $term_field = variable_get('yml_tools_term_field', '');
    $descr_field = variable_get('yml_tools_descr_field', '');

    $this->checkSettings();

    $node_types = $this->getNodeTypes();

    $query = new EntityFieldQuery();

    $nodes = array();
    foreach ($node_types as $type) {
      $tmp_nodes = array();
      $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', $type)
        ->propertyCondition('status', 1);
      $result = $query->execute();

      if (!empty($result['node'])) {
        $nids = array_keys($result['node']);
        $tmp_nodes = node_load_multiple($nids);
      }
      $nodes = array_merge($nodes, (array) $tmp_nodes);
    }

    foreach ($nodes as $id => $node) {
      $validate_price = (float) $this->getProductPrice($node);
      if (empty($validate_price)) {
        continue;
      }

      $term = field_get_items('node', $node, $term_field);
      if (isset($term[0])) {
        $term = $term[0];
      }

      $description = field_get_items('node', $node, $descr_field);
      if (isset($description[0])) {
        $description = check_plain(strip_tags($description[0]['value']));
      }

      $title = check_plain($node->title);
      $url = url('node/' . $node->nid, array('absolute' => TRUE));
      $image = $this->getProductImage($node);
      $price = round((float) $this->getProductPrice($node), 2);
      $model = $this->getProductSKU($node);

      $products[$id] = (object) array(
        'title' => $title,
        'url' => $url,
        'category' => $term['tid'],
        'price' => $price,
        'description' => $description,
        'image' => $image,
        'model' => $model,
      );
    }
    return $products;
  }

  /**
   * Generate XML.
   */
  public function getXML() {
    $products_objects = $this->getProducts();
    $categories_objects = $this->getCategories();

    // Generate XML for each category and product from templates.
    $categories = '';
    foreach ($categories_objects as $category) {
      $category->name = check_plain($category->name);
      // Set the first parent category as the primary parent.
      if (!empty($category->parents[0])) {
        $category->parent = $category->parents[0];
      }
      $categories .= theme('yml_tools_category', array(
        'category' => $category,
      ));
    }
    $products = '';
    foreach ($products_objects as $product) {
      $products .= theme('yml_tools_product', array(
        'product' => $product,
        'currency' => $this->getCurrency(),
      ));
    }

    // Return XML.
    header('Content-type: application/xhtml+xml; charset=utf-8');
    echo theme('yml_tools_xml', array(
      'products' => $products,
      'categories' => $categories,
      'currency' => $this->getCurrency(),
    ));
    exit();
  }

  /**
   * Check module settings. Call exception if no settings.
   */
  static public function checkSettings() {
    $products = array();
    if (variable_get('yml_tools_vid', '') == '') {
      die('Please select primary vocabulary on YML export settings page!');
    }

    $ctypes = variable_get('yml_tools_types', array('product' => 'product'));
    $enabled_ctypes = array();
    foreach ($ctypes as $type_name => $enabled) {
      if ($enabled) {
        $enabled_ctypes[$type_name] = $type_name;
      }
    }

    if (empty($enabled_ctypes)) {
      die('Please select at least one node type on YML export settings page!');
    }
  }
}

/**
 * Ubercart Store Class
 */
class YmlUbercartStore extends YmlStoreType implements YmlStoreTypeInterface {
  /**
   * Implements class constructor.
   */
  public function __construct() {
    $this->type = YML_TOOLS_STORE_TYPE_UBERCART;
  }

  /**
   * Get content types of product.
   */
  public function getContentTypes() {
    $ctypes = uc_product_types();
    $ctypes = array_combine($ctypes, $ctypes);
    return array_keys($ctypes);
  }

  /**
   * Get store curency.
   */
  public function getCurrency() {
    return variable_get('uc_currency_code', 'USD');
  }

  /**
   * Get price for single product.
   *
   * @param object $product
   *   Product node.
   *
   * @return float
   *   Product price
   */
  public function getProductPrice($product) {
    return $product->sell_price;
  }

  /**
   * Get image for single product.
   *
   * @param object $product
   *   Product node
   *
   * @return string
   *   Product image url
   */
  public function getProductImage($product) {
    $image_field = variable_get('yml_tools_image_field', '');
    $path = '';
    $image = field_get_items('node', $product, $image_field);
    if (isset($image[0])) {
      $path = file_create_url($image[0]['uri']);
    }
    return $path;
  }

  /**
   * Get SKU for single product.
   *
   * @param object $product
   *   Product node
   *
   * @return string
   *   Product SKU
   */
  public function getProductSKU($product) {
    return check_plain($product->model);
  }
}

/**
 * Drupal Commerce Store Class
 */
class YmlCommerceStore extends ymlStoreType implements YmlStoreTypeInterface  {
  /**
   * Implements class constructor.
   */
  public function __construct() {
    $this->type = YML_TOOLS_STORE_TYPE_COMMERCE;
  }

  /**
   * Get content types of product.
   */
  public function getContentTypes() {
    $ctypes = commerce_product_types();
    $ctypes = array_keys($ctypes);
    $ctypes = array_combine($ctypes, $ctypes);
    return $ctypes;
  }

  /**
   * Get store curency.
   */
  public function getCurrency() {
    return commerce_default_currency();
  }

  /**
   * Get image for single product.
   *
   * @param object $product
   *   Product node
   *
   * @return string
   *   Product image url
   */
  public function getProductImage($product) {
    $image_field = variable_get('yml_tools_image_field', '');
    $path = '';
    $product = commerce_product_load($product->nid);
    $image = field_get_items('commerce_product', $product, $image_field);
    if (isset($image[0])) {
      $path = file_create_url($image[0]['uri']);
    }
    return $path;
  }

  /**
   * Get price for single product.
   *
   * @param object $product
   *   Product node.
   *
   * @return float
   *   Product price
   */
  public function getProductPrice($product) {
    $product = commerce_product_load($product->nid);
    $price = commerce_product_calculate_sell_price($product);
    $price = commerce_currency_amount_to_decimal($price['amount'], $price['currency_code']);
    return $price;
  }

  /**
   * Get SKU for single product.
   *
   * @param object $product
   *   Product node
   *
   * @return string
   *   Product SKU
   */
  public function getProductSKU($product) {
    $product_wrapper = entity_metadata_wrapper('node', $product);
    return $product_wrapper->field_product->get(0)->sku->value();
  }
}

class YmlStore {
  /**
   * Factory method for create store.
   *
   * @return YmlUbercartStore|YmlCommerceStore|NULL
   *   Return store class
   */
  static public function createStore() {
    if (module_exists('uc_store')) {
      return new YmlUbercartStore();
    }
    elseif (module_exists('commerce')) {
      return new YmlCommerceStore();
    }
    else {
      return NULL;
    }
  }
}
