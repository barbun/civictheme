<?php

/**
 * @file
 * Filter-related functions.
 */

/**
 * Theme suggestions for all civictheme filter elements.
 */
function _civictheme_theme_suggestions__civictheme_filter(array &$suggestions, array $variables, $type = NULL) {
  if (empty($variables['element']['#type'])) {
    return;
  }

  $type = $type ?? $variables['element']['#type'];

  if (!empty($variables['element']['#ct-group-filter'])) {
    $suggestions[] = $type . '__civictheme_dropdown_filter';
  }
  elseif (!empty($variables['element']['#ct-single-filter'])) {
    $suggestions[] = $type . '__civictheme_single_filter';
  }
}

/**
 * Theme suggestions for civictheme basic filter elements.
 */
function _civictheme_theme_suggestions__civictheme_single_filter(array &$suggestions, array $variables) {
  // For civictheme basic filter both radios / checkboxes and checkbox / radio
  // require custom templates.
  if (in_array($variables['element']['#type'], ['radio', 'checkbox'])
    && isset($variables['element']['#attributes']['#ct-single-filter'])) {
    $suggestions[] = 'form_element__civictheme_filter_chip';
  }
}
