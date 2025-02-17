<?php

/**
 * @file
 * Banner component alterations.
 */

use Drupal\civictheme\CivicthemeConstants;
use Drupal\Core\Cache\Cache;
use Drupal\node\NodeInterface;

/**
 * Pre-process for Programmatic Block - Banner block.
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 * @SuppressWarnings(PHPMD.CyclomaticComplexity)
 */
function _civictheme_preprocess_block__civictheme_banner(&$variables) {
  if ($variables['base_plugin_id'] != 'block_content') {
    return;
  }

  $entity = $variables['elements']['content']['#block_content'];
  if ($entity->bundle() != 'civictheme_banner' || ($entity->hasField('field_c_b_type') && $entity->field_c_b_type->isEmpty())) {
    return;
  }

  $variables['content'] = NULL;

  // Cache tags.
  /** @var \Drupal\node\Entity\Node $node */
  $node = \Drupal::routeMatch()->getParameter('node');
  if ($node instanceof NodeInterface) {
    $variables['#cache']['tags'] = Cache::mergeTags($entity->getCacheTags(), $node->getCacheTags());
  }
  $variables['#cache']['contexts'][] = 'url.path';
  $variables['#cache']['contexts'][] = 'url.query_args';

  $variables['type'] = CivicthemeConstants::BANNER_TYPE_DEFAULT;

  $variables['theme'] = civictheme_get_field_theme_value($entity);

  if (civictheme_field_has_value($entity, 'field_c_b_background_image')) {
    $variables['background_image'] = civictheme_media_get_url_from_field($entity, 'field_c_b_background_image');

    if (civictheme_field_has_value($entity, 'field_c_b_blend_mode')) {
      $variables['background_image_blend_mode'] = civictheme_get_field_value($entity, 'field_c_b_blend_mode', TRUE);
    }
  }

  // Combining Drupal block classes with banner.
  if (!empty($variables['attributes']['class'])) {
    $variables['modifier_class'] = implode(' ', $variables['attributes']['class']);
    unset($variables['attributes']['class']);
  }

  _civictheme_preprocess_civictheme_banner_title($variables);
  _civictheme_preprocess_civictheme_banner_node($variables);
  _civictheme_preprocess_civictheme_banner_breadcrumb($variables);
  _civictheme_preprocess_civictheme_banner_section($variables);

  civictheme_expose_migration_metadata__banner($variables);

  if ($variables['type'] == CivicthemeConstants::BANNER_TYPE_LARGE) {
    $variables['is_decorative'] = TRUE;
  }

  // Explicitly defined variables to allow additional preprocessing.
  $variables['content_top1'] = NULL;
  $variables['content_top2'] = NULL;
  $variables['content_top3'] = NULL;
  $variables['content_bottom'] = NULL;
}

/**
 * Pre-process banner section.
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 */
function _civictheme_preprocess_civictheme_banner_section(&$variables) {
  // Site section.
  /** @var \Drupal\node\Entity\Node $node */
  $node = \Drupal::routeMatch()->getParameter('node');

  if (!is_null($node)) {
    $the_section = civictheme_get_field_value($node, 'field_c_n_site_section', TRUE);
    $variables['section'] = isset($the_section) ? $the_section->getName() : '';
  }
}

/**
 * Pre-process banner title.
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 */
function _civictheme_preprocess_civictheme_banner_title(&$variables) {
  // Banner title.
  /** @var \Symfony\Component\HttpFoundation\Request $request */
  $request = \Drupal::request();
  /** @var \Drupal\Core\Routing\RouteMatchInterface $route_match */
  $route_match = \Drupal::routeMatch();
  /** @var \Drupal\Core\Controller\TitleResolverInterface $title_resolver */
  $title_resolver = \Drupal::service('title_resolver');

  $page_title = $title_resolver->getTitle($request, $route_match->getRouteObject());

  /** @var \Drupal\node\Entity\Node $node */
  $node = \Drupal::routeMatch()->getParameter('node');
  $node_revision = \Drupal::routeMatch()->getParameter('node_revision');

  if ($node_revision !== NULL) {
    $node = $node_revision instanceof NodeInterface ? $node_revision : node_revision_load($node_revision);
  }

  if (!is_null($node)) {
    $page_title = civictheme_get_field_value($node, 'field_c_n_banner_title') ?? $page_title;
  }

  $variables['title'] = $page_title ?? '';
}

/**
 * Pre-process banner breadcrumb.
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 * @SuppressWarnings(PHPMD.CyclomaticComplexity)
 */
function _civictheme_preprocess_civictheme_banner_breadcrumb(&$variables) {
  // Disable breadcrumb on homepage.
  if (\Drupal::service('path.matcher')->isFrontPage()) {
    return;
  }

  /** @var \Drupal\node\Entity\Node $node */
  $node = \Drupal::routeMatch()->getParameter('node');
  $node_revision = \Drupal::routeMatch()->getParameter('node_revision');

  if ($node_revision !== NULL) {
    $node = $node_revision instanceof NodeInterface ? $node_revision : node_revision_load($node_revision);
  }

  // Breadcrumb visibility.
  $hide_breadcrumb = FALSE;
  if (!is_null($node)) {
    $hide_breadcrumb = civictheme_get_field_value($node, 'field_c_n_banner_hide_breadcrumb') ?? FALSE;
  }

  if (!$hide_breadcrumb) {
    /** @var \Drupal\Core\Routing\RouteMatch $route_match */
    $route_match = \Drupal::getContainer()->get('current_route_match');
    /** @var \Drupal\Core\Breadcrumb\Breadcrumb|null $breadcrumb */
    $breadcrumb = \Drupal::service('breadcrumb')->build($route_match);
    if ($breadcrumb) {
      $variables['breadcrumb'] = $breadcrumb->toRenderable();
      if (isset($variables['theme'])) {
        // Changing to CivicTheme theme to pass other variables to template.
        $variables['breadcrumb']['#theme'] = 'civictheme_breadcrumb';
        $variables['breadcrumb']['#display_theme'] = $variables['theme'];
      }
    }
  }
}

/**
 * Pre-process banner for all nodes.
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 */
function _civictheme_preprocess_civictheme_banner_node(&$variables) {
  /** @var \Drupal\node\Entity\Node $node */
  $node = \Drupal::routeMatch()->getParameter('node');
  $node_revision = \Drupal::routeMatch()->getParameter('node_revision');

  if ($node_revision !== NULL) {
    $node = $node_revision instanceof NodeInterface ? $node_revision : node_revision_load($node_revision);
  }

  if (is_null($node)) {
    return;
  }

  $type = civictheme_get_field_value($node, 'field_c_n_banner_type', TRUE, CivicthemeConstants::INHERIT);
  $variables['type'] = $type == CivicthemeConstants::INHERIT ? ($variables['type'] ?? CivicthemeConstants::BANNER_TYPE_DEFAULT) : $type;

  $theme = civictheme_get_field_value($node, 'field_c_n_banner_theme', TRUE, CivicthemeConstants::INHERIT);
  $variables['theme'] = $theme == CivicthemeConstants::INHERIT ? ($variables['theme'] ?? CivicthemeConstants::BANNER_THEME_DEFAULT) : $theme;

  $is_front = \Drupal::service('path.matcher')->isFrontPage();
  $variables['is_front'] = $is_front;

  _civictheme_preprocess_civictheme_banner_last_updated($variables, $node);
  _civictheme_preprocess_civictheme_banner_background_image($variables, $node);
  _civictheme_preprocess_civictheme_banner_featured_image($variables, $node);
  _civictheme_preprocess_civictheme_banner_components($variables, $node);
}

/**
 * Preprocess banner last updated date.
 */
function _civictheme_preprocess_civictheme_banner_last_updated(&$variables, $node) {
  $final_date = _civictheme_preprocess_civictheme_banner_get_last_updated($node);

  if (isset($final_date)) {
    $variables['content_middle'][] = [
      '#theme' => 'civictheme_basic_content',
      '#component_theme' => $variables['theme'] ?? NULL,
      '#content' => '<p class="ct-banner__last_updated">Last updated: ' . $final_date . '</p>',
    ];
  }
}

/**
 * Helper to determine last updated date for a page.
 */
function _civictheme_preprocess_civictheme_banner_get_last_updated($node) {
  $last_updated = civictheme_get_field_value($node, 'field_c_n_show_last_updated');
  $custom_updated = civictheme_get_field_value($node, 'field_c_n_custom_last_updated', TRUE);

  $final_date = NULL;

  if ($last_updated && $custom_updated) {
    $final_date = civictheme_format_short_date($custom_updated->get('value')->getDateTime());
  }

  elseif ($last_updated) {
    $final_date = \Drupal::service('date.formatter')->format($node->getChangedTime(), 'civictheme_short_date');
  }

  return $final_date;
}

/**
 * Preprocess banner background image.
 *
 * @SuppressWarnings(PHPMD.ElseExpression)
 */
function _civictheme_preprocess_civictheme_banner_background_image(&$variables, $node) {
  $background_image = civictheme_get_field_value($node, 'field_c_n_banner_background', TRUE);
  if ($background_image !== NULL) {
    $variables['background_image'] = civictheme_media_get_url($background_image);
    if (civictheme_field_has_value($node, 'field_c_n_blend_mode')) {
      $variables['background_image_blend_mode'] = civictheme_get_field_value($node, 'field_c_n_blend_mode', TRUE);
    }
  }
}

/**
 * Preprocess banner featured image.
 */
function _civictheme_preprocess_civictheme_banner_featured_image(&$variables, $node) {
  $featured_image = civictheme_get_field_value($node, 'field_c_n_banner_featured_image', TRUE);
  if ($featured_image !== NULL) {
    $image = civictheme_get_field_value($featured_image, 'field_c_m_image', TRUE);
    $variables['featured_image']['src'] = civictheme_media_get_url($featured_image);
    $variables['featured_image']['alt'] = $image !== NULL ? $image->getValue()['alt'] : '';
  }
}

/**
 * Preprocess banner components.
 */
function _civictheme_preprocess_civictheme_banner_components(&$variables, $node) {
  $banner_components = civictheme_get_field_value($node, 'field_c_n_banner_components');
  if ($banner_components) {
    $render_controller = \Drupal::entityTypeManager()->getViewBuilder('paragraph');
    $variables['content'][] = $render_controller->viewMultiple($banner_components, 'default');
  }

  // Banner content below components.
  $banner_below_components = civictheme_get_field_value($node, 'field_c_n_banner_components_bott');
  if ($banner_below_components) {
    $render_controller = \Drupal::entityTypeManager()->getViewBuilder('paragraph');
    $variables['content_below'][] = $render_controller->viewMultiple($banner_below_components, 'default');
  }
}
