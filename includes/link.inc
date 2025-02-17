<?php

/**
 * @file
 * Link related functions.
 */

use Drupal\Component\Utility\Html;
use Drupal\Component\Utility\UrlHelper;

/**
 * Process HTML links within an HTML content string.
 *
 * @param string $html
 *   The string to be processed.
 * @param string $base_url
 *   The hostname to process.
 * @param bool $new_window
 *   Flag to open all links in the new window.
 * @param bool $external_new_window
 *   Flag to open external links in the new window.
 * @param array $override_domains
 *   Array of override domains.
 *
 * @return string
 *   The processed HTML string.
 *
 * @SuppressWarnings(PHPMD.CyclomaticComplexity)
 * @SuppressWarnings(PHPMD.NPathComplexity)
 * @SuppressWarnings(PHPMD.StaticAccess)
 * @SuppressWarnings(PHPMD.BooleanArgumentFlag)
 */
function _civictheme_process_html_content_links($html, $base_url, $new_window = FALSE, $external_new_window = FALSE, array $override_domains = []) {
  $class_prefix = 'ct-';
  $link_class = 'ct-content-link ct-theme-light';
  $link_external_class = 'ct-content-link--external';

  $email_pattern = '~(^|[\s\.,;\n\(])([a-zA-Z0-9._+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4})~';
  $html = preg_replace($email_pattern, '$1<a href="mailto:$2">$2</a>', $html);

  $dom = Html::load($html);

  if (!$dom) {
    return $html;
  }

  $anchors = $dom->getElementsByTagName('a');
  if (!$anchors->count()) {
    return $html;
  }

  foreach ($anchors as $anchor) {
    // Apply only to anchors with links.
    if (!$anchor->hasAttribute('href')) {
      continue;
    }

    // Apply only if there are no classes prefixed with a prefix.
    $classes = $anchor->hasAttribute('class') ? ($anchor->getAttribute('class') ?: '') : '';

    $url = $anchor->getAttribute('href');
    $is_external = civictheme_url_is_external($url, $base_url, $override_domains);

    if ((!str_contains($classes, $class_prefix)) && (!str_contains($classes, $link_class))) {
      $classes .= ' ' . $link_class;
    }

    if ($is_external && !str_contains($classes, $link_external_class)) {
      $classes .= ' ' . $link_external_class;
    }

    if (!$anchor->hasAttribute('target')) {
      if ($new_window || ($is_external && $external_new_window)) {
        $anchor->setAttribute('target', '_blank');
      }
    }

    $anchor->setAttribute('class', trim($classes));
  }

  return Html::serialize($dom);
}

/**
 * Check if the provided link is external.
 *
 * @param string $url
 *   The url to be validated.
 * @param string $base_url
 *   The base URL string to check against, such as "http://example.com/".
 * @param array $override_domains
 *   Array of overridden domains.
 *
 * @return bool
 *   TRUE if the provided URL is external, FALSE if not external or not a valid
 *   URL.
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 */
function civictheme_url_is_external($url, $base_url, array $override_domains = []) {
  if (empty($url) || !UrlHelper::isValid($url)) {
    return FALSE;
  }

  $url_parts = parse_url($url);
  if (!empty($url_parts['scheme']) && empty($url_parts['host'])) {
    return FALSE;
  }

  if (UrlHelper::isExternal($url)) {
    $override_domains[] = $base_url;
    foreach ($override_domains as $override_domain) {
      $override_domain = _civictheme_external_link_normalize_domain($override_domain);

      if (!UrlHelper::isValid($override_domain, TRUE)) {
        continue;
      }

      if (UrlHelper::externalIsLocal($url, $override_domain)) {
        return FALSE;
      }
    }

    return TRUE;
  }

  return FALSE;
}

/**
 * Normalise domain.
 */
function _civictheme_external_link_normalize_domain($domain) {
  // Check that the part of the domain before first dot has protocol and
  // add one if it does not.
  if (!str_contains(substr($domain, 0, strpos($domain, '.')), ':')) {
    $domain = 'https://' . (str_starts_with($domain, '//') ? str_replace('//', '', $domain) : $domain);
  }

  return $domain;
}
