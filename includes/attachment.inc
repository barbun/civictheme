<?php

/**
 * @file
 * CivicTheme Attachment paragraph component.
 */

use Drupal\file\Entity\File;

/**
 * Implements template_preprocess_paragraph().
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 */
function civictheme_preprocess_paragraph__civictheme_attachment(&$variables) {
  /** @var \Drupal\paragraphs\Entity\Paragraph $paragraph */
  $paragraph = $variables['paragraph'];

  $links = NULL;
  $attachments = civictheme_get_field_value($paragraph, 'field_c_p_attachments') ?? [];
  foreach ($attachments as $attachment) {
    $fid = $attachment->getSource()->getSourceFieldValue($attachment);
    /** @var \Drupal\file\FileInterface $file */
    $file = $fid ? File::load($fid) : NULL;
    if (!empty($file)) {
      $url = $file->createFileUrl(FALSE);
      $file_size = format_size($file->getSize());
      $mime_type = $file->getMimeType();
      $icon = _civictheme_attachment_file_icon_map($mime_type);
      $file_extension = pathinfo($file->getFileUri(), PATHINFO_EXTENSION);
      $links[] = [
        'url' => $url,
        'text' => t('@name', [
          '@name' => $file->label(),
        ]),
        'ext' => $file_extension ?? '',
        'size' => $file_size ?? '',
        'icon' => $icon,
        'last_updated' => \Drupal::service('date.formatter')->format($attachment->getChangedTime(), 'civictheme_short_date'),
      ];
    }
  }

  $variables['links'] = $links;
}

/**
 * File icon symbol provider.
 *
 * @param string $mime_type
 *   File mimetype to get the respected symbol.
 *
 * @return string
 *   Return Symbol.
 *
 * @SuppressWarnings(PHPMD.CyclomaticComplexity)
 */
function _civictheme_attachment_file_icon_map($mime_type) {
  switch ($mime_type) {
    // Word document types.
    case 'application/msword':
    case 'application/vnd.ms-word.document.macroEnabled.12':
    case 'application/vnd.oasis.opendocument.text':
    case 'application/vnd.oasis.opendocument.text-template':
    case 'application/vnd.oasis.opendocument.text-master':
    case 'application/vnd.oasis.opendocument.text-web':
    case 'application/vnd.openxmlformats-officedocument.wordprocessingml.document':
    case 'application/vnd.stardivision.writer':
    case 'application/vnd.sun.xml.writer':
    case 'application/vnd.sun.xml.writer.template':
    case 'application/vnd.sun.xml.writer.global':
    case 'application/vnd.wordperfect':
    case 'application/x-abiword':
    case 'application/x-applix-word':
    case 'application/x-kword':
    case 'application/x-kword-crypt':
      return 'word-file';

    // Spreadsheet document types.
    case 'application/vnd.ms-excel':
    case 'application/vnd.ms-excel.sheet.macroEnabled.12':
    case 'application/vnd.oasis.opendocument.spreadsheet':
    case 'application/vnd.oasis.opendocument.spreadsheet-template':
    case 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet':
    case 'application/vnd.stardivision.calc':
    case 'application/vnd.sun.xml.calc':
    case 'application/vnd.sun.xml.calc.template':
    case 'application/vnd.lotus-1-2-3':
    case 'application/x-applix-spreadsheet':
    case 'application/x-gnumeric':
    case 'application/x-kspread':
    case 'application/x-kspread-crypt':
      return 'download-file';

    // Presentation document types.
    case 'application/vnd.ms-powerpoint':
    case 'application/vnd.ms-powerpoint.presentation.macroEnabled.12':
    case 'application/vnd.oasis.opendocument.presentation':
    case 'application/vnd.oasis.opendocument.presentation-template':
    case 'application/vnd.openxmlformats-officedocument.presentationml.presentation':
    case 'application/vnd.stardivision.impress':
    case 'application/vnd.sun.xml.impress':
    case 'application/vnd.sun.xml.impress.template':
    case 'application/x-kpresenter':
      return 'download-file';

    // Acrobat types.
    case 'application/pdf':
    case 'application/x-pdf':
    case 'applications/vnd.pdf':
    case 'text/pdf':
    case 'text/x-pdf':
      return 'pdf-file';

    default:
      return 'download-file';
  }
}
