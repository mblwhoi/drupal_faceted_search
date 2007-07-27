// $Id$

/**
 * Provides tooltips with subcategories when hovering categories, through AHAH.
 */ 
jQuery.facetedSearchUI = {
  isActive : false,
  activate : function() {
    if (jQuery.facetedSearchUI.isActive || !this.href) {
      return;
    }
    
    // Extract the url to load from the category's url.
    var url = null;
    var facetKey = null;
    var facetId = null;
    jQuery(this).parents('.faceted-search-facet').each(function() {
      // Extract the facet key and id from the class of the category's parent.
      var matches = jQuery(this).attr('class').match(/faceted-search-facet--([^ ]+)--([^ ]+)/);
      if (matches.length > 0) {
        facetKey = matches[1];
        facetId = matches[2];
      }
    });
    if (facetKey && facetId) {
      // Derive the url from the category's url, which also contains the search text.
      url = this.href.replace(/\/results\/na\//, '/categories/' + facetKey + '%3A' + facetId + '/');
    }

    // If we've been able to extract an url, perform the request.
    if (url != this.href) {
      jQuery.facetedSearchUI.isActive = true;
      
      // Request HTML chunk for the tooltip.
      jQuery.get(url, null, jQuery.facetedSearchUI.show);

      // Prepare the tooptip
      var elemPosition = jQuery(this).offset();
      var elemWidth = jQuery(this).outerWidth();
      var windowWidth = jQuery(window).width();
      var tooltip = jQuery('#faceted-search-tooltip');
      var tooltipWidth = tooltip.outerWidth();
      tooltip.css('top', elemPosition.top + 'px');
      if (elemPosition.left + elemWidth + tooltipWidth + 10 > windowWidth) {
        tooltip.css('left', (elemPosition.left - tooltipWidth - 10) + 'px');
        tooltip.addClass('faceted-search-tooltip-left'); // Might be useful for theming.
      }
      else {
        tooltip.css('left', (elemPosition.left + elemWidth + 10) + 'px');
        tooltip.addClass('faceted-search-tooltip-right'); // Might be useful for theming.
      }
    }
  },
  deactivate : function() {
    if (jQuery.facetedSearchUI.isActive) {
      jQuery.facetedSearchUI.isActive = false;
      jQuery('#faceted-search-tooltip').hide().empty().removeClass('faceted-search-tooltip-left').removeClass('faceted-search-tooltip-right');
    }
  },
  show : function(chunk) {
    // Avoid showing the chunk if it's too late (e.g. after the mouse has moved
    // out of the category).
    if (jQuery.facetedSearchUI.isActive && chunk.length > 0) {
      jQuery('#faceted-search-tooltip').empty().append(chunk).show();
    }
  }
};

if (Drupal.jsEnabled) {
  jQuery(function() {
    // Insert the tooltip block.
    jQuery('body').append('<div id="faceted-search-tooltip"></div>');
    // Bind hover behavior on category links.
    jQuery('.faceted-search-category a').each(function() {
      // TODO: Some delay before sending request... This will save some requests. Consider jquery.hoverIntent.js
      jQuery(this).hover(jQuery.facetedSearchUI.activate, jQuery.facetedSearchUI.deactivate);
    });
    // Bind click behavior to force closing the tooptip, if ever needed.
    jQuery(this).click(jQuery.facetedSearchUI.deactivate);
  });
}

