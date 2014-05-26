<?php

interface ymlStoreTypeInterface {
  /**
   * Get store curency
   */
  public function getCurrency();

  public function getProductPrice($product);

  public function getProductSKU($product);
}

/**
 * Abstract class for store type
 */
abstract class ymlStoreType {
  protected $type;

  /**
   * Return store type
   */
  public function getType() {
    return $this->type;
  }

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
   * Get store categories
   */
  public function getCategories() {
    $categories = taxonomy_get_tree(variable_get('yml_tools_vid', ''));
    return $categories;
  }

  /**
   * Get store products
   */
  public function getProducts() {
    $term_field = variable_get('yml_tools_term_field', '');
    $descr_field = variable_get('yml_tools_descr_field', '');
    $image_field = variable_get('yml_tools_image_field', '');

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
      $nodes = array_merge($nodes, (array)$tmp_nodes);
    }

  	foreach ($nodes as $id => $node){
  	  $validate_price = (int)$this->getProductPrice($node);
      if (empty($validate_price)) {
        //continue;
      }

      $term = field_get_items('node', $node, $term_field);
    	if (isset($term[0])) {
    	 $term = $term[0];
    	}

    	$description = field_get_items('node', $node, $descr_field);
    	if (isset($description[0])) {
    	 $description = check_plain(strip_tags($description[0]['value']));
    	}

    	$image = field_get_items('node', $node, $image_field);
    	if (isset($image[0])) {
    	 $image = file_create_url($image[0]['uri']);
    	}
      $title = check_plain($node->title);
      $url = url('node/' . $node -> nid, array('absolute' => TRUE));
      $price = $this->getProductPrice($node);
      $model = $this->getProductSKU($node);

    	$products[$id] = (object)array(
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
   * Generate XML
   */
  public function getXML() {
    $products_objects = $this->getProducts();
    $categories_objects = $this->getCategories();

    // Generate XML for each category and product from templates
    $categories = '';
    foreach ($categories_objects as $category) {
      $category->name = check_plain($category->name);
      // Set the first parent category as the primary parent
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

    // Return XML
    header('Content-type: application/xhtml+xml; charset=utf-8');
    echo theme('yml_tools_xml', array(
      'products' => $products,
      'categories' => $categories,
      'currency' => $this->getCurrency(),
    ));
    exit();
  }

  static function checkSettings() {
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
class ymlUbercartStore extends ymlStoreType implements ymlStoreTypeInterface {
  public function ymlUbercartStore() {
    $this->type = YML_STORE_TYPE_UBERCART;
  }

  public function getCurrency() {
    return variable_get('uc_currency_code', 'USD');
  }

  public function getProductPrice($product) {
      return $node->sell_price;
  }

  public function getProductSKU($product) {
      return check_plain($product->model);
  }
}

/**
 * Drupal Commerce Store Class
 */
class ymlCommerceStore extends ymlStoreType implements ymlStoreTypeInterface  {
  public function ymlCommerceStore() {
    $this->type = YML_STORE_TYPE_COMMERCE;
  }

  public function getCurrency() {
    return commerce_default_currency();
  }

  public function getProductPrice($product) {
    $product = commerce_product_load($product->nid);
    $price = commerce_product_calculate_sell_price($product);
    $price = commerce_currency_amount_to_decimal($price['amount'], $price['currency_code']);
    return $price;
  }

  public function getProductSKU($product) {
    $product_wrapper = entity_metadata_wrapper('node', $product);
    return $product_wrapper->field_product->get(0)->sku->value();
  }
}

class ymlStore {
  static function createStore() {
    if (module_exists('uc_store')) {
      return new ymlUbercartStore();
    }
    elseif (module_exists('commerce')) {
      return new ymlCommerceStore();
    }
    else return null;
  }
}
