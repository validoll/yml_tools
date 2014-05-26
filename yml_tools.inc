<?php

/**
 * Abstract class for store type
 */
abstarct class ymlStore {
  protected $type;

  public getType() {
    return $this->type;
  }
}

/**
 * Ubercart Store Class
 */
class ymlUbercartStore extends ymlStore {
  public function ymlUbercartStore() {
    $this->type = "Ubercart";
  }
}

/**
 * Drupal Commerce Store Class
 */
class ymlCommerceStore extends ymlStore {
  public function ymlCommerceStore() {
    $this->type = "Commerce";
  }
}
