(function($) {
    var java, program_input;
    var langs = ['Java', 'C', 'C++'];
    $(document).ready(function() {
	$('.form-item-java').addClass('grid-8 suffix-4');
	$('.form-item-programinput').addClass('grid-8 suffix-4');
	if($.inArray($('#programming-language').val(), langs) > -1) {
	    java = $('.java').codemirror({
		mode: "clike",
		lineNumbers: true,
		viewportMargin: Infinity,
                theme: "blackboard"
	    });
        }

	if($('#programming-language').val() == 'Python') {
	    java = $('.java').codemirror({
		mode: {name: "python",
		       version: 2,
		       singleLineStringErrors: false},
		lineNumbers: true,
		matchBrackets: true,
		viewportMargin: Infinity,
		theme: "blackboard",
		autofocus: true
	    });
        }

	if($('#programming-language').val() == 'SQL') {
	    java = $('.java').codemirror({
		mode: 'text/x-mariadb',
		indentWithTabs: true,
		smartIndent: true,
		lineNumbers: true,
		matchBrackets : true,
		theme: "blackboard",
		autofocus: true
	    });
        }

	if($('#programming-language').val() == 'Bash') {
	    java = $('.java').codemirror({
		mode: 'shell',
		lineNumbers: true,
		matchBrackets: true,
		theme: "blackboard",
		autofocus: true
	    });
        }

	if($('#programming-language').val() == 'Ruby') {
	    java = $('.java').codemirror({
		mode: "text/x-ruby",
		tabMode: "indent",
		matchBrackets: true,
		indentUnit: 4,
		lineNumbers: true,
		viewportMargin: Infinity,
		theme: "blackboard",
		autofocus: true
	    });

        }

	program_input = $('.program-input').codemirror({
	    mode: "clike", theme: "blackboard", indentUnit: 0
	});
    });
    Drupal.behaviors.ideone = {
	attach: function() {
	    Drupal.ajax['edit-submit'].beforeSerialize = function () {
		$('#codingbat').val(java.getValue());
		$('#program-input').val(program_input.getValue());
	    }
	}
    };

})(jQuery);
