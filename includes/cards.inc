<?php

/**
 * @file
 * Card theme alterations.
 */

use Drupal\civictheme\CivicthemeConstants;

/**
 * Generic setter of global variables for nodes as card renders.
 */
function _civictheme_preprocess_node__card(&$variables) {
  /** @var \Drupal\node\NodeInterface $node */
  $node = $variables['node'];
  $type = $node->getType();
  unset($variables['date']);
  $type_callback = '_civictheme_preprocess_node__' . $type . '__summary';
  if (function_exists($type_callback)) {
    $type_callback($variables);
  }
  $variables['title'] = $node->getTitle();
  civictheme_attributes_to_modifier_class($variables);
  _civictheme_preprocess_node__theme($variables);
}

/**
 * Preprocess.
 */
function _civictheme_preprocess_node__civictheme_event_card(&$variables) {
  _civictheme_preprocess_node__card($variables);
  _civictheme_preprocess_node__date($variables);
  _civictheme_preprocess_node__thumbnail($variables, 'civictheme_promo_card');
  _civictheme_preprocess_node__topics($variables);
}

/**
 * Pre-process reference Theme fields for CivicTheme Nodes.
 */
function _civictheme_preprocess_node__theme(&$variables) {
  /** @var \Drupal\node\NodeInterface $node */
  $node = $variables['node'];

  if (isset($node->_referringItem)) {
    /** @var \Drupal\paragraphs\Entity\Paragraph $referring_paragraph */
    $referring_paragraph = $node->_referringItem->getEntity() ?? NULL;
    $variables['theme'] = civictheme_get_field_theme_value($referring_paragraph);
  }

  // For when theme is set by view or other third party.
  if (isset($node->civictheme_theme)) {
    $variables['theme'] = $node->civictheme_theme;
  }
}

/**
 * Pre-process a Node for navigation_card view mode.
 */
function _civictheme_preprocess_node__civictheme_navigation_card(&$variables) {
  _civictheme_preprocess_node__card($variables);
  _civictheme_preprocess_node__thumbnail($variables, 'civictheme_navigation_card');
}

/**
 * Pre-process a Node for civictheme_subject view mode.
 */
function _civictheme_preprocess_node__civictheme_promo_card(&$variables) {
  /** @var \Drupal\node\NodeInterface $node */
  $node = $variables['node'];
  $variables['date'] = \Drupal::service('date.formatter')->format($node->getCreatedTime(), 'civictheme_short_date');
  _civictheme_preprocess_node__card($variables);
  _civictheme_preprocess_node__date($variables);
  _civictheme_preprocess_node__thumbnail($variables, 'civictheme_promo_card');
  _civictheme_preprocess_node__topics($variables);
}

/**
 * Pre-process a Node for civictheme_subject_card view mode.
 */
function _civictheme_preprocess_node__civictheme_subject_card(&$variables) {
  _civictheme_preprocess_node__card($variables);
  _civictheme_preprocess_node__thumbnail($variables);
}

/**
 * Pre-process date field.
 */
function _civictheme_preprocess_node__date(&$variables) {
  /** @var \Drupal\node\NodeInterface $node */
  $node = $variables['node'];
  $date_range = civictheme_get_field_value($node, 'field_c_n_date', TRUE);
  if ($date_range) {
    $variables['date'] = civictheme_format_short_date($date_range->get('value')->getString());
  }
}

/**
 * Topic field preprocessor.
 */
function _civictheme_preprocess_node__topics(&$variables) {
  /** @var \Drupal\node\NodeInterface $node */
  $node = $variables['node'];
  $tags = [];
  $topic_terms = civictheme_get_field_value($node, 'field_c_n_topics') ?? [];
  foreach ($topic_terms as $topic_term) {
    $tags[] = $topic_term->getName();
  }
  $variables['tags'] = $tags;
}

/**
 * Pre-process image thumbnail for nodes.
 */
function _civictheme_preprocess_node__thumbnail(&$variables, $image_style = NULL) {
  /** @var \Drupal\node\NodeInterface $node */
  $node = $variables['node'];
  $media = civictheme_get_field_value($node, 'field_c_n_thumbnail', TRUE);
  if ($media) {
    $variables['image'] = civictheme_media_image_process_variables($media, $image_style);
  }
}

/**
 * Pre-process Summary for civictheme_page nodes.
 */
function _civictheme_preprocess_node__civictheme_page__summary(&$variables) {
  /** @var \Drupal\node\NodeInterface $node */
  $node = $variables['node'];
  $summary = civictheme_get_field_value($node, 'field_c_n_summary', TRUE);
  if ($summary) {
    $length = CivicthemeConstants::CARD_SUMMARY_DEFAULT_LENGTH;
    if (!empty($variables['view_mode']) && str_ends_with($variables['view_mode'], '_card')) {
      $length = civictheme_get_theme_config_manager()->loadForComponent(str_replace('civictheme_', '', $variables['view_mode']), 'summary_length', CivicthemeConstants::CARD_SUMMARY_DEFAULT_LENGTH);
    }
    $variables['summary'] = text_summary($summary, NULL, $length);
  }
}

/**
 * Pre-process Summary for civictheme_event.
 */
function _civictheme_preprocess_node__civictheme_event__summary(&$variables) {
  /** @var \Drupal\node\NodeInterface $node */
  $node = $variables['node'];
  $summary = civictheme_get_field_value($node, 'field_c_n_summary', TRUE);
  if ($summary) {
    $length = CivicthemeConstants::CARD_SUMMARY_DEFAULT_LENGTH;
    if (!empty($variables['view_mode']) && str_ends_with($variables['view_mode'], '_card')) {
      $length = civictheme_get_theme_config_manager()->loadForComponent(str_replace('civictheme_', '', $variables['view_mode']), 'summary_length', CivicthemeConstants::CARD_SUMMARY_DEFAULT_LENGTH);
    }
    $variables['summary'] = text_summary($summary, NULL, $length);
  }
}
