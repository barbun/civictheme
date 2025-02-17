<?php

/**
 * @file
 * CivicTheme quick links paragraph component.
 */

/**
 * Implements template_preprocess_paragraph().
 */
function civictheme_preprocess_paragraph__civictheme_quick_links(&$variables) {
  /** @var \Drupal\paragraphs\Entity\Paragraph $paragraph */
  $paragraph = $variables['paragraph'];
  /** @var \Drupal\link\Plugin\Field\FieldType\LinkItem[]|null $links */
  $links = civictheme_get_field_value($paragraph, 'field_c_p_links') ?? [];
  foreach ($links as $link) {
    $variables['links'][] = [
      'url' => $link->getUrl()->toString(),
      'text' => $link->get('title')->getString(),
      'is_external' => $link->isExternal(),
      'is_new_window' => $link->isExternal(),
    ];
  }
}
